
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Proxy.h"
#include "WPILib.h"
#include "Robot.h"
#include "nivision.h"
#include "Target2.h"

#define DPRINTF if(true)dprintf								//debugging info
#define TPRINTF if(true)dprintf								//testing info
#define GPRINTF if(true)dprintf								//Let's GO!

static int Countup(Image* ImageToCount);					//Print out number of particles

#define BAD_CAMERA (false)

static int ReturnReport(
		Image* ProcessedImage, 
		int numParticles, 
		corner_t DesiredValue, 
		ParticleAnalysisReport* Report);


int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep, int btnInput)
/*	Description: This function takes an image and analyzes it for the target.
	Return Value:
		0: failed (seeing too many rectangles or none)
		1-4: sees 1-4 rect
*/
{
	//create destination/editable image
	Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);
	DPRINTF(LOG_INFO, "\nLOOK HERE\n");
	
	
#if (!BAD_CAMERA)
	/*Step 1: Color Threshold 
	(Red: 0-255, Green: 174-255,  Blue: 210-255)
	int imaqColorThreshold(Image* dest, const Image* source, int replaceValue, ColorMode mode, const Range* plane1Range, const Range* plane2Range, const Range* plane3Range); */
	/* Setup Threshold Values */
	int NP;
	const Range RR = {100,255};
	const Range GR = {0  ,255};
	const Range BR = {12 ,255};
	int thresholdcheck;
	thresholdcheck=imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR);
	if(FailCheck(thresholdcheck, "Color Threshold Failed %i")) {return 0; } else {DPRINTF(LOG_INFO, "Thresholded");}
	if (!Countup(ProcessedImage)) return 0;
	
	/*Step 1.5: Get rid of itty-bitty bits*/
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	ParticleFilterCriteria2 CRIT1 = {IMAQ_MT_AREA, 10  , 76800, FALSE, FALSE};
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT1, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
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
	
	/* Step 3: Basic Morphology Erosion (to clean up) */
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	if(FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle), "Erosion")) {return 0;} else {DPRINTF(LOG_INFO, "Eroded");}
	Countup(ProcessedImage);
	
	
	/*Step 4: Particle Filters (Area: min.5%, max100% of image)
	int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
	/* Setup for Particle Filter */
	ParticleFilterCriteria2 CRIT[3] = {{IMAQ_MT_AREA, 150  , 76800, FALSE, FALSE}, 
			                           {IMAQ_MT_COMPACTNESS_FACTOR, 0.0, 0.5, FALSE, FALSE},
    								   {IMAQ_MT_COMPACTNESS_FACTOR, 0.8, 1.0, FALSE, FALSE}};
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
	Countup(ProcessedImage);
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[1], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered 2");}
	Countup(ProcessedImage);
	
	/* Step 5: Convex hull it */
	if(FailCheck(imaqConvexHull(ProcessedImage, ProcessedImage, TRUE), "Convex Hull")) 
	{return 0; } 
	else 
	{DPRINTF(LOG_INFO, "Convexed");}
	
	/* Step 6: Take out the particles that aren't filled in enough */
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[2], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
	
	
	
	
#endif
	
#if BAD_CAMERA
	/*Step 1: Color Threshold */
		/* Setup Threshold Values */
		const Range RR = {0  ,255};
		const Range GR = {0  ,255};
		const Range BR = {150,255};
		if(FailCheck(imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR), "Color Threshold Failed %i")) {return 0; } else {DPRINTF(LOG_INFO, "Thresholded");}
		if (!Countup(ProcessedImage)) return 0;
		
		/*Step 2: Basic Morphology Dilation
		int imaqMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement); */
		/* Setup for Morphology*/
			static int kernelvalues[9] = {1,1,1,1,1,1,1,1,1};
			const StructuringElement StructEle = {3, 3, FALSE, &kernelvalues[0]};
			imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
		if (FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle), "Dilation 1 %f")) {return 0; } else {DPRINTF(LOG_INFO, "Dilated");}
		free(StructEle.kernel);
		Countup(ProcessedImage);
		
		/* Step 3: Basic Morphology Erosion (to clean up) */
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
		
		
		/*Step 4: Particle Filter (Area: min.5%, max100% of image)
		int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
		/* Setup for Particle Filter */
		int IMAQheight;
		int IMAQwidth;
		imaqGetImageSize(ProcessedImage, &IMAQwidth, &IMAQheight);
		ParticleFilterCriteria2 CRIT[3] = {{IMAQ_MT_AREA,             100  , IMAQwidth*IMAQheight, FALSE, FALSE}, 
				                           {IMAQ_MT_COMPACTNESS_FACTOR, 0.0, 0.7, FALSE, FALSE},
	    								   {IMAQ_MT_COMPACTNESS_FACTOR, 0.7, 1.0, FALSE, FALSE}};
		const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
		int NP;
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 2, &OPTS, NULL, &NP), "Filter Particles 1 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
		Countup(ProcessedImage);
		
		/* Step 5: Convex hull it */
		if(FailCheck(imaqConvexHull(ProcessedImage, ProcessedImage, TRUE), "Convex Hull")) 
		{return 0; } 
		else 
		{DPRINTF(LOG_INFO, "Convexed");}
		
		/* Step 6: Take out the particles that aren't filled in enough */
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[2], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
		Countup(ProcessedImage);
#endif
	/* Step 7: Apply logic */
	int numParticles=Countup(ProcessedImage);
	GPRINTF(LOG_INFO, "Number particles found: %i", numParticles);
	if(numParticles==0) return 0;
	else if(numParticles==1 || numParticles==2 || numParticles==3 || numParticles==4)
	{
		ReturnReport(ProcessedImage, numParticles, TOP_MOST,    &ParticleRep[TOP_MOST]   );
		ReturnReport(ProcessedImage, numParticles, LEFT_MOST,   &ParticleRep[LEFT_MOST]  );
		ReturnReport(ProcessedImage, numParticles, RIGHT_MOST,  &ParticleRep[RIGHT_MOST] );
		ReturnReport(ProcessedImage, numParticles, BOTTOM_MOST, &ParticleRep[BOTTOM_MOST]);
		
		GPRINTF(LOG_INFO, "TOP:    %f   \t%f", ParticleRep[TOP_MOST].center_mass_x_normalized,    ParticleRep[TOP_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "LEFT:   %f   \t%f", ParticleRep[LEFT_MOST].center_mass_x_normalized,   ParticleRep[LEFT_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "RIGHT:  %f   \t%f", ParticleRep[RIGHT_MOST].center_mass_x_normalized,  ParticleRep[RIGHT_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "BOTTOM: %f   \t%f", ParticleRep[BOTTOM_MOST].center_mass_x_normalized, ParticleRep[BOTTOM_MOST].center_mass_y_normalized);

		GPRINTF(LOG_INFO, "TOP:    %i   \t%i", ParticleRep[TOP_MOST].center_mass_x,    ParticleRep[TOP_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "LEFT:   %i   \t%i", ParticleRep[LEFT_MOST].center_mass_x,   ParticleRep[LEFT_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "RIGHT:  %i   \t%i", ParticleRep[RIGHT_MOST].center_mass_x,  ParticleRep[RIGHT_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "BOTTOM: %i   \t%i", ParticleRep[BOTTOM_MOST].center_mass_x, ParticleRep[BOTTOM_MOST].center_mass_y);
	}
	else if(numParticles>4)
	{
		int BestDVIndex;
		double BestDV=0;
		GPRINTF(LOG_INFO, "More than 4 found!");
		Countup(ProcessedImage);
		for (int i = 0; i < numParticles; i++) 
		{
			double testmeasure;
			imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_AREA, &testmeasure);
			if (testmeasure > BestDV)
			{ BestDV=testmeasure; BestDVIndex = i; }
		}
		ParticleFilterCriteria2 CRIT2 = {IMAQ_MT_AREA, (.9)*BestDV, 76800, FALSE, FALSE};
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT2, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
		numParticles= Countup(ProcessedImage);
		
		ReturnReport(ProcessedImage, numParticles, TOP_MOST,    &ParticleRep[TOP_MOST]   );
		ReturnReport(ProcessedImage, numParticles, LEFT_MOST,   &ParticleRep[LEFT_MOST]  );
		ReturnReport(ProcessedImage, numParticles, RIGHT_MOST,  &ParticleRep[RIGHT_MOST] );
		ReturnReport(ProcessedImage, numParticles, BOTTOM_MOST, &ParticleRep[BOTTOM_MOST]);
		
		TPRINTF(LOG_INFO, "TOP:    %f   \t%f", ParticleRep[TOP_MOST].center_mass_x_normalized,    ParticleRep[TOP_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "LEFT:   %f   \t%f", ParticleRep[LEFT_MOST].center_mass_x_normalized,   ParticleRep[LEFT_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "RIGHT:  %f   \t%f", ParticleRep[RIGHT_MOST].center_mass_x_normalized,  ParticleRep[RIGHT_MOST].center_mass_y_normalized);
		TPRINTF(LOG_INFO, "BOTTOM: %f   \t%f", ParticleRep[BOTTOM_MOST].center_mass_x_normalized, ParticleRep[BOTTOM_MOST].center_mass_y_normalized);
	
		GPRINTF(LOG_INFO, "TOP:    %i   \t%i", ParticleRep[TOP_MOST].center_mass_x,    ParticleRep[TOP_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "LEFT:   %i   \t%i", ParticleRep[LEFT_MOST].center_mass_x,   ParticleRep[LEFT_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "RIGHT:  %i   \t%i", ParticleRep[RIGHT_MOST].center_mass_x,  ParticleRep[RIGHT_MOST].center_mass_y);
		GPRINTF(LOG_INFO, "BOTTOM: %i   \t%i", ParticleRep[BOTTOM_MOST].center_mass_x, ParticleRep[BOTTOM_MOST].center_mass_y);
	
	}
	
	return numParticles;
}


bool FailCheck(int Returned, char* Description)
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

static int ReturnReport(Image* ProcessedImage, int numParticles, corner_t DesiredValue, ParticleAnalysisReport* Report)
{
	int BestDVIndex;
	double BestDV;
	switch (DesiredValue)//Find the rectangle with the desired value
	{
		case TOP_MOST:
			BestDV=1000;
			for (int i = 0; i < numParticles; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_CENTER_OF_MASS_Y, &testmeasure);
				if (testmeasure < BestDV)
				{ BestDV=testmeasure; BestDVIndex = i; }
			}
		break;
		case LEFT_MOST:
			BestDV=1000;
			for (int i = 0; i < numParticles; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_CENTER_OF_MASS_X, &testmeasure);
				if (testmeasure < BestDV)
				{ BestDV=testmeasure; BestDVIndex = i; }
			}
		break;
		case RIGHT_MOST:
			BestDV=0;
			for (int i = 0; i < numParticles; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_CENTER_OF_MASS_X, &testmeasure);
				if (testmeasure > BestDV)
				{ BestDV=testmeasure; BestDVIndex = i; }
			}
			break;
		case BOTTOM_MOST:
			BestDV=0;
			for (int i = 0; i < numParticles; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_CENTER_OF_MASS_Y, &testmeasure);
				if (testmeasure > BestDV)
				{ BestDV=testmeasure; BestDVIndex = i; }
			}
		break;
		default:
			printf("\n\nSome fool put a bad value into M's image processing program!!\n\n");
		break;
	}
	frcParticleAnalysis(ProcessedImage, BestDVIndex, Report);
	return 0;
}



