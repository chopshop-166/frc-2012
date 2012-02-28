
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Proxy.h"
#include "WPILib.h"
#include "Robot.h"
#include "nivision.h"
#include "Target2.h"
#include "Defines.h"

#define DPRINTF if(false)dprintf			//debugging info
#define TPRINTF if(false)dprintf			//testing info
#define GPRINTF if(false)dprintf			//Let's GO!
#define STEP4 (true)						//In the shortened version, shorten it more.
#define SHORTENED (true)					//IN THE CASE that you want to shorten the
				//image processing, but decrease accuracy... true!

static int Countup(Image* ImageToCount);	//Print out number of particles

static int ReturnReport(Image* ProcessedImage, int numParticles, corner_t DesiredValue, ParticleAnalysisReport* Report);
											//Returns ParticleAnalysisReports to CameraTask

int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep, int btnInput)
{
	//create destination/editable image
	Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);
	DPRINTF(LOG_INFO, "\nLOOK HERE\n");

/*****************************************************************
                     Isolate the targets
 *****************************************************************/
#if SHORTENED
	/*Step 1: Color Threshold */
	int NP;
	const Range RR = {115,135};
	const Range GR = {0  ,255};
	const Range BR = {12 ,255};
	int thresholdcheck;
	thresholdcheck=imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR);
	if(FailCheck(thresholdcheck, "Color Threshold Failed %i")) 
	{
		frcDispose(ProcessedImage);
		return 0; 
	} else {DPRINTF(LOG_INFO, "Thresholded");}
	if (!Countup(ProcessedImage)) 
	{
		frcDispose(ProcessedImage);
		return 0;
	}
	
	/* Step 2: Convex hull it */
	if(FailCheck(imaqConvexHull(ProcessedImage, ProcessedImage, TRUE), "Convex Hull")) 
	{
		imaqDispose(ProcessedImage);
		return 0; 
	} 
	else 
	{DPRINTF(LOG_INFO, "Convexed");}
	
	/* Step 3: Particle Filters: Compactness (eliminate not-dense particles) */
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	ParticleFilterCriteria2 CRIT= {IMAQ_MT_COMPACTNESS_FACTOR, 0.8, 1.0, FALSE, FALSE};
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
	{
		frcDispose(ProcessedImage);
		return 0; 
	} else {DPRINTF(LOG_INFO, "Filtered");}
	if (!Countup(ProcessedImage)) 
	{
		frcDispose(ProcessedImage);
		return 0;
	}
#if STEP4
	/* Step 4: Delete particles that are much smaller than the largest. */
	int BestDVIndex;
	double BestDV=0;
	GPRINTF(LOG_INFO, "More than 4 found!");
	int numParticles=Countup(ProcessedImage);
	for (int i = 0; i < numParticles; i++) //determine largest particle
	{
		double testmeasure;
		imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_AREA, &testmeasure);
		if (testmeasure > BestDV)
		{ BestDV=testmeasure; BestDVIndex = i; }
	}
	//Filter
	ParticleFilterCriteria2 CRIT2 = {IMAQ_MT_AREA, (.7)*BestDV, 76800, FALSE, FALSE};
	if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT2, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
	{
		frcDispose(ProcessedImage);
		return 0; 
	} else {DPRINTF(LOG_INFO, "Filtered");}
#endif
#endif
	
#if !SHORTENED
	/*Step 1: Color Threshold */
		int NP;
		const Range RR = {115,135};
		const Range GR = {0  ,255};
		const Range BR = {8 ,255};
		int thresholdcheck;
		thresholdcheck=imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR);
		if(FailCheck(thresholdcheck, "Color Threshold Failed %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Thresholded");}
		if (!Countup(ProcessedImage)) 
		{
			frcDispose(ProcessedImage);
			return 0;
		}
		
		/*Step 2: Use an area filter to eliminate tiny/massive particles */
		const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
		ParticleFilterCriteria2 CRIT1 = {IMAQ_MT_AREA, 10  , 38400, FALSE, FALSE};
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT1, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Filtered");}
		Countup(ProcessedImage);
		
		/*Step 3: Basic Morphology Dilation 
		static int kernelvalues[9] = {1,1,1,
									  1,1,1,
									  1,1,1};
		const StructuringElement StructEle = {3, 3, FALSE, &kernelvalues[0]};
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
		if (FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle), "Dilation 1 %f")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Dilated");}
		Countup(ProcessedImage);
		
		Step 5: Basic Morphology Erosion 
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
		imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
		if(FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle), "Erosion")) 
		{
			frcDispose(ProcessedImage);
			return 0;
		} else {DPRINTF(LOG_INFO, "Eroded");}
		Countup(ProcessedImage);	*/
		
		/* Step 6: Particle Filters: Area and Compactness (eliminate dense particles) */
		ParticleFilterCriteria2 CRIT[3] = {{IMAQ_MT_AREA, 150  , 76800, FALSE, FALSE}, 
				                           {IMAQ_MT_COMPACTNESS_FACTOR, 0.0, 0.5, FALSE, FALSE},
	    								   {IMAQ_MT_COMPACTNESS_FACTOR, 0.8, 1.0, FALSE, FALSE}};
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Filtered");}
		Countup(ProcessedImage);
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[1], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Filtered 2");}
		Countup(ProcessedImage);
		
		/* Step 7: Convex hull it */
		if(FailCheck(imaqConvexHull(ProcessedImage, ProcessedImage, TRUE), "Convex Hull")) 
		{
			imaqDispose(ProcessedImage);
			return 0; 
		} 
		else 
		{DPRINTF(LOG_INFO, "Convexed");}
		
		/* Step 8: Particle Filter: Compactness (eliminate not-dense particles) */
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[2], 1, &OPTS, NULL, &NP), "Filter Particles 2 %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Filtered");}
		
		/* Step 9: Delete particles that are much smaller than the largest. */
		int BestDVIndex;
		double BestDV=0;
		GPRINTF(LOG_INFO, "More than 4 found!");
		int numParticles=Countup(ProcessedImage);
		for (int i = 0; i < numParticles; i++) //determine largest particle
		{
			double testmeasure;
			imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_AREA, &testmeasure);
			if (testmeasure > BestDV)
			{ BestDV=testmeasure; BestDVIndex = i; }
		}
		//Filter
		ParticleFilterCriteria2 CRIT2 = {IMAQ_MT_AREA, (.9)*BestDV, 76800, FALSE, FALSE};
		if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT2, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
		{
			frcDispose(ProcessedImage);
			return 0; 
		} else {DPRINTF(LOG_INFO, "Filtered");}
		numParticles= Countup(ProcessedImage);
		
		/* Step 10: Filter out particles if more than 4 */
		if(numParticles>4)
		{
			GPRINTF(LOG_INFO, "%i Particles found at end! Eliminating...", numParticles);
			
			double ParticleAreas[numParticles];
			for(int x=0; x<numParticles;x++)
			{
				imaqMeasureParticle(ProcessedImage, x, 0, IMAQ_MT_AREA, &ParticleAreas[x]);
			}
			
			double SortedParticleAreas[numParticles];
			SortedParticleAreas[0] = ParticleAreas[0];
			for(int p=1; p<numParticles; p++)
			{
				for (int y=0; y<numParticles; y++)
				{
					if (SortedParticleAreas[y] > ParticleAreas[p])
					{
						for (int z=numParticles-1; z>y; z--)
						{
							SortedParticleAreas[z] = SortedParticleAreas[z-1];
						}
						SortedParticleAreas[y] = ParticleAreas[p];
						break;
					}
				}
			}
			
			 //Filter
			ParticleFilterCriteria2 CRIT3 = {IMAQ_MT_AREA, SortedParticleAreas[3]-1, 76800, FALSE, FALSE};
			if(FailCheck(imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT3, 1, &OPTS, NULL, &NP), "Filter Particles 1 %i")) 
			{
				frcDispose(ProcessedImage);
				return 0; 
			} else {DPRINTF(LOG_INFO, "Filtered");}
			/*LOGIC IS UNTESTED*/
		}
		

#endif
	
	
/*****************************************************************
                     Examine the targets
 *****************************************************************/
	/* Step 1: Use number of particles to determine what's what */
	numParticles=Countup(ProcessedImage); //Count Particles
	GPRINTF(LOG_INFO, "Number particles found: %i", numParticles);
	
	if(numParticles==0) //If none found, return nothing
	{
		frcDispose(ProcessedImage);
		return 0; 
	}
	else
	{
		//Return information
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
	frcDispose(ProcessedImage);
	return numParticles;
}


bool FailCheck(int Returned, char* Description)
/* Description: Prints out errors.
 * Syntax to call:
 * if(FailCheck(imaqFxn, "Describe function Failed %i")) return 0; */
{
	if(!Returned)//Upon failure, NI Vision Functions return 0.
	{
		int errCode = GetLastVisionError();
		DPRINTF(LOG_INFO, Description);
		DPRINTF(LOG_INFO, "Error code: ", errCode);
		char *errString = GetVisionErrorText(errCode);
		DPRINTF(LOG_INFO, "errString= %s", errString);
		return true;
	}
	else//so NI Vision function worked
	{
		return false;
	}
}

static int Countup(Image* ImageToCount)
/* Description: counts up the number of particles in an image.
 * If DPRINTF is enabled, it prints the number of particles.
 * It returns the number of particles. */
{
	int numParticles;
	imaqCountParticles(ImageToCount, TRUE, &numParticles);
	DPRINTF(LOG_INFO, "\t\tnumParticles: %i", numParticles);
	return numParticles;
}

static int ReturnReport(Image* ProcessedImage, int numParticles, corner_t DesiredValue, ParticleAnalysisReport* Report)
/* Description: takes an image and chosen target to fill in a ParticleAnalysisReport.  
 * This returns a 0 regardless of successfulness. */
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



