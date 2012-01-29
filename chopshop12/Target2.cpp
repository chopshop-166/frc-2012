#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Target2.h"
#include "Proxy.h"

#define DPRINTF if(true)dprintf								//debugging info
#define TPRINTF if(true)dprintf								//testing info

static bool FailCheck(int Returned, char* Description);		//Print out errors
static int Countup(Image* ImageToCount);					//Print out number of particles
#if I_SEE_4_TARGETS
static int ReturnReport(
		Image* ProcessedImage, 
		int* index, 
		corner_t DesiredValue, 
		ParticleAnalysisReport* Report);
#endif

int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep)
/*	Description: This function takes an image and analyzes it for the target.
	Return Value:
		0: failed (seeing too many rectangles or none)
		1-4: sees 1-4 rect
*/
{
	//create destination/editable image

	Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);
	DPRINTF(LOG_INFO, "\nLOOK HERE\n");
	
	/*Step 1: Color Threshold 
	(Red: 0-255, Green: 174-255,  Blue: 210-255)
	int imaqColorThreshold(Image* dest, const Image* source, int replaceValue, ColorMode mode, const Range* plane1Range, const Range* plane2Range, const Range* plane3Range); */
	/* Setup Threshold Values */
	const Range RR = {0  ,255};
	const Range GR = {0  ,255};
	const Range BR = {140,255};
	int thresholdcheck;
	thresholdcheck=imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR);
	if(FailCheck(thresholdcheck, "Color Threshold Failed %i")) {return 0; } else {DPRINTF(LOG_INFO, "Thresholded");}
	Countup(ProcessedImage);
	
	/*Step 2: Basic Morphology Dilation
	int imaqMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement); */
	/* Setup for Morphology*/
		static int kernelvalues[9] = {1,1,1,1,1,1,1,1,1};
		const StructuringElement StructEle = {3, 3, FALSE, &kernelvalues[0]};
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	if (FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle), "Dilation 1 %f")) {return 0; } else {DPRINTF(LOG_INFO, "Dilated");}
	free(StructEle.kernel);
	Countup(ProcessedImage);
	
	/*Step 3: Fill Holes
	int imaqFillHoles(Image* dest, const Image* source, int connectivity8); */
	if(FailCheck(imaqFillHoles(ProcessedImage, ProcessedImage, TRUE), "Fill Holes %i")) {return 0; } else {DPRINTF(LOG_INFO, "Holes Filled");}
	if(FailCheck(imaqLabel2(ProcessedImage, ProcessedImage, TRUE, NULL), "Label")) return 0;
	if(FailCheck(imaqConvexHull(ProcessedImage, ProcessedImage, TRUE), "Convex Hull")) return 0;
	
	
	/*Step 4: Particle Filter (Area: min.5%, max100% of image)
	int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
	/* Setup for Particle Filter */
	const ParticleFilterCriteria2 CRIT = {IMAQ_MT_AREA_BY_IMAGE_AREA, 1, 100, FALSE, FALSE};
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	int NP;
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT, 1, &OPTS, NULL, &NP), "Filter Particles %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
	Countup(ProcessedImage);
	
	/*Step 5: Convert to 16-bit image, max contrast
	int imaqLookup(Image* dest, const Image* source, const short* table, const Image* mask); */
	/* Setup for Lookup Table */
	short TBL[256];
		TBL[0]=0;
		for(int i=1;i<256;i++) TBL[i]=255;
	if(FailCheck(imaqLookup(ProcessedImage, ProcessedImage, &TBL[0], NULL), "Lookup Table %i"))  {return 0; } else {DPRINTF(LOG_INFO, "Look up Tabled");}
	int NumPart=Countup(ProcessedImage);
	
#if (I_SEE_4_TARGETS)
	/* Step 6: Get particle(s) that are targets*/
	int index[4];
	if (NumPart>4) //we need to sort to find the four with the best value
	{
		for (int j=0; j<4; j++) index[j]=0;
		double measure[4];
		for (int e=0; e<4; e++) measure[e]=0;
		
		for(int i=0; i<NumPart; i++)
		{
			double testmeasure;
			imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_COMPACTNESS_FACTOR, &testmeasure);
			if (testmeasure>measure[0])
			{
				index[3]=index[2]; measure[3]=measure[2];
				index[2]=index[1]; measure[2]=measure[1];
				index[1]=index[0]; measure[1]=measure[0];
				index[0]=i;        measure[0]=testmeasure;
			}
			else if (testmeasure>measure[1])
			{
				index[3]=index[2]; measure[3]=measure[2];
				index[2]=index[1]; measure[2]=measure[1];
				index[1]=i;        measure[1]=testmeasure;
			}
			else if (testmeasure>measure[2])
			{
				index[3]=index[2]; measure[3]=measure[2];
				index[2]=i;        measure[2]=testmeasure;
			}
			else if (testmeasure>measure[3])
			{
				index[3]=i;        measure[3]=testmeasure;
			}
		}
	}
	else if (NumPart==4)
	{
		for(int i=0; i<4; i++)
			index[i]=i;
	}
	else
	{
		DPRINTF(LOG_INFO, "Cannot see enough!");
		return 0;
	}
	
	/* Step 7: Return reports*/
	ReturnReport(ProcessedImage, &index[0], TOP_MOST,    &ParticleRep[TOP_MOST]   );
	ReturnReport(ProcessedImage, &index[0], LEFT_MOST,   &ParticleRep[LEFT_MOST]  );
	ReturnReport(ProcessedImage, &index[0], RIGHT_MOST,  &ParticleRep[RIGHT_MOST] );
	ReturnReport(ProcessedImage, &index[0], BOTTOM_MOST, &ParticleRep[BOTTOM_MOST]);
	
	DPRINTF(LOG_INFO, "TOP:    %f\t%f", ParticleRep[TOP_MOST].center_mass_x_normalized,    ParticleRep[TOP_MOST].center_mass_y_normalized);
	DPRINTF(LOG_INFO, "LEFT:   %f\t%f", ParticleRep[LEFT_MOST].center_mass_x_normalized,   ParticleRep[LEFT_MOST].center_mass_y_normalized);
	DPRINTF(LOG_INFO, "RIGHT:  %f\t%f", ParticleRep[RIGHT_MOST].center_mass_x_normalized,  ParticleRep[RIGHT_MOST].center_mass_y_normalized);
	DPRINTF(LOG_INFO, "BOTTOM: %f\t%f", ParticleRep[BOTTOM_MOST].center_mass_x_normalized, ParticleRep[BOTTOM_MOST].center_mass_y_normalized);
		
	
	return Countup(ProcessedImage);
	
	

#endif

#if (!I_SEE_4_TARGETS)
	/* Step 6: find desired particle */
	int DesiredParticleIndex;
	double BestValSoFar=0;
	for(int i=0;i<NumPart;i++)
	{
		double testmeasure;
		imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_COMPACTNESS_FACTOR, &testmeasure);
		if(testmeasure>BestValSoFar) 
		{
			DesiredParticleIndex=i; 
			BestValSoFar=testmeasure;
		}
	}
	
/* Step 7: Find best particle and analyze */
	frcParticleAnalysis(ProcessedImage, DesiredParticleIndex, ParticleRep);
	DPRINTF(LOG_INFO, "Particle Analysis: %f,\t%f", (*ParticleRep).center_mass_x_normalized, (*ParticleRep).center_mass_y_normalized);
	return Countup(ProcessedImage);
	
#endif
}


static bool FailCheck(int Returned, char* Description)
/*	Description: Prints out errors.
	Syntax to call:
		if(FailCheck(imaqFxn, "Describe function Failed %i")) return 0;
*/
{/* Error report logic */
	if(!Returned)
	{
		int errCode = GetLastVisionError();
		DPRINTF(LOG_INFO, Description);
		DPRINTF(LOG_INFO, "Error code: ", errCode);
		char *errString = GetVisionErrorText(errCode);
		DPRINTF(LOG_INFO, "errString= %s", errString);
		return true;
	}
	else
	{
		return false;
	}
}

static int Countup(Image* ImageToCount)
{
	int numParticles;
	imaqCountParticles(ImageToCount, TRUE, &numParticles);
	DPRINTF(LOG_INFO, "\t\tnumParticles: %i", numParticles);
	return numParticles;
}

#if I_SEE_4_TARGETS
static int ReturnReport(Image* ProcessedImage, int* index, corner_t DesiredValue, ParticleAnalysisReport* Report)
{
	int BestDVIndex;
	double BestDV=0;
	switch (DesiredValue)//Find the rectangle with the desired value
	{
		case TOP_MOST:
			for (int i = 0; i < 4; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_TOP, &testmeasure);
				if (testmeasure < BestDV)
				{ BestDV=testmeasure; BestDVIndex = index[i]; }
			}
		break;
		case LEFT_MOST:
			for (int i = 0; i < 4; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_RIGHT, &testmeasure);
				if (testmeasure < BestDV)
				{ BestDV=testmeasure; BestDVIndex = index[i]; }
			}
		break;
		case RIGHT_MOST:
			for (int i = 0; i < 4; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_RIGHT, &testmeasure);
				if (testmeasure > BestDV)
				{ BestDV=testmeasure; BestDVIndex = index[i]; }
			}
		case BOTTOM_MOST:
			for (int i = 0; i < 4; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_TOP, &testmeasure);
				if (testmeasure > BestDV)
				{ BestDV=testmeasure; BestDVIndex = index[i]; }
			}
		break;
		default:
			printf("\n\nSome fool put a bad value into M's image processing program!!\n\n");
		break;
	}
	frcParticleAnalysis(ProcessedImage, BestDVIndex, Report);
	return 0;
}
#endif



