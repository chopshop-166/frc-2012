#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Target2.h"
#include "Proxy.h"

#define DPRINTF if(false)dprintf								//debugging info
#define TPRINTF if(true)dprintf								//testing info

static bool FailCheck(int Returned, char* Description);		//Print out errors
static int Countup(Image* ImageToCount);					//Print out number of particles

static int ReturnReport(
		Image* ProcessedImage, 
		int numParticles, 
		corner_t DesiredValue, 
		ParticleAnalysisReport* Report);


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
	const Range BR = {20 ,255};
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
	
	/*Step 4: Particle Filter (Area: min.5%, max100% of image)
	int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
	/* Setup for Particle Filter */
	const ParticleFilterCriteria2 CRIT[3] = {{IMAQ_MT_NUMBER_OF_HOLES,    1  , 2  , FALSE, FALSE}, 
			                                 {IMAQ_MT_COMPACTNESS_FACTOR, 0.2, 0.5, FALSE, FALSE},
    										 {IMAQ_MT_COMPACTNESS_FACTOR, 0.6, 1.0, FALSE, FALSE}};
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	int NP;
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 2, &OPTS, NULL, &NP), "Filter Particles 1 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
	Countup(ProcessedImage);
	
	/* Step 5: Fill up holes */
	if(FailCheck(imaqFillHoles(ProcessedImage, ProcessedImage, TRUE), "Fill Holes %i")) {return 0; } else {DPRINTF(LOG_INFO, "Holes Filled");}
	
	/* Step 6: Take out the particles that aren't filled in enough */
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[2], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) {return 0; } else {DPRINTF(LOG_INFO, "Filtered");}
	Countup(ProcessedImage);
	
	/* Step 7.0: Check particles */
	int numP = Countup(ProcessedImage);
	ParticleAnalysisReport PARTest[numP];
	for (int h=0; h<numP; h++)
	{
		frcParticleAnalysis(ProcessedImage, h, &PARTest[h]);
		TPRINTF(LOG_INFO, "Particle: %i   \t%i", PARTest[h].center_mass_x, PARTest[h].center_mass_y);
	}
	
	/* Step 7: Apply logic */
	int numParticles=Countup(ProcessedImage);
	TPRINTF(LOG_INFO, "Number particles found: %i", numParticles);
	if(numParticles>4 || numParticles==0) return 0;
	
	ReturnReport(ProcessedImage, numParticles, TOP_MOST,    &ParticleRep[TOP_MOST]   );
	ReturnReport(ProcessedImage, numParticles, LEFT_MOST,   &ParticleRep[LEFT_MOST]  );
	ReturnReport(ProcessedImage, numParticles, RIGHT_MOST,  &ParticleRep[RIGHT_MOST] );
	ReturnReport(ProcessedImage, numParticles, BOTTOM_MOST, &ParticleRep[BOTTOM_MOST]);
	
	TPRINTF(LOG_INFO, "TOP:    %f   \t%f", ParticleRep[TOP_MOST].center_mass_x_normalized,    ParticleRep[TOP_MOST].center_mass_y_normalized);
	TPRINTF(LOG_INFO, "LEFT:   %f   \t%f", ParticleRep[LEFT_MOST].center_mass_x_normalized,   ParticleRep[LEFT_MOST].center_mass_y_normalized);
	TPRINTF(LOG_INFO, "RIGHT:  %f   \t%f", ParticleRep[RIGHT_MOST].center_mass_x_normalized,  ParticleRep[RIGHT_MOST].center_mass_y_normalized);
	TPRINTF(LOG_INFO, "BOTTOM: %f   \t%f", ParticleRep[BOTTOM_MOST].center_mass_x_normalized, ParticleRep[BOTTOM_MOST].center_mass_y_normalized);

	TPRINTF(LOG_INFO, "TOP:    %i   \t%i", ParticleRep[TOP_MOST].center_mass_x,    ParticleRep[TOP_MOST].center_mass_y);
	TPRINTF(LOG_INFO, "LEFT:   %i   \t%i", ParticleRep[LEFT_MOST].center_mass_x,   ParticleRep[LEFT_MOST].center_mass_y);
	TPRINTF(LOG_INFO, "RIGHT:  %i   \t%i", ParticleRep[RIGHT_MOST].center_mass_x,  ParticleRep[RIGHT_MOST].center_mass_y);
	TPRINTF(LOG_INFO, "BOTTOM: %i   \t%i", ParticleRep[BOTTOM_MOST].center_mass_x, ParticleRep[BOTTOM_MOST].center_mass_y);
	
	return numParticles;
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



