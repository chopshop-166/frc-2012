/*Purpose: find balls to shoot*/
#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Target2.h"
#include "Proxy.h"

double ProcessMyImageForBalls(Image* CameraSource)
{
	Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);
	
	/*Step 1: HSL Color plane extraction*/
	Range HR, SR, LR;
		HR.minValue=5  ; HR.maxValue= 14;
		SR.minValue=131; SR.maxValue=200;
		LR.minValue= 86; LR.maxValue=153;
	imaqColorThreshold(ProcessedImage, CameraSource, 255, IMAQ_HSL, &HR, &SR, &LR); 
	
	/*Step 2: Smooth out bits (clean up)*/
	StructuringElement StructEle;
		StructEle.matrixCols = 3; StructEle.matrixRows = 3; StructEle.hexa = FALSE;
		StructEle.kernel = (int*) malloc(9 * sizeof(int));
		for(int s=0;s<9;s++) StructEle.kernel[s] = 1;
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	free(StructEle.kernel);
	return 1;
	
	/*Step 3: Filter out pointless particles */
	int IMAQheight;
	int IMAQwidth;
	imaqGetImageSize(ProcessedImage, &IMAQwidth, &IMAQheight);
	ParticleFilterCriteria2 CRIT[3] = {{IMAQ_MT_AREA,             1000  , IMAQwidth*IMAQheight, FALSE, FALSE}, 
			                           {IMAQ_MT_COMPACTNESS_FACTOR, 0.0, 0.4, FALSE, FALSE},
    								   {IMAQ_MT_COMPACTNESS_FACTOR, 0.6, 1.0, FALSE, FALSE}};
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	int NP;
	imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 2, &OPTS, NULL, &NP);
	
	/* Step 4: Dilate again to clean up */
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	
	/* Step 5: Convex hull */
	imaqConvexHull(ProcessedImage, ProcessedImage, TRUE);
	
	/*Step 6: Find best particle */
	int numParticles;
	imaqCountParticles(ProcessedImage, TRUE, &numParticles);
	int BestDVIndex;
	double BestDV;
	double testmeasure;
	for (int i = 0; i < numParticles; i++) 
	{
		imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_AREA, &testmeasure);
		if (testmeasure < BestDV)
		{ BestDV=testmeasure; BestDVIndex = i; }
	}
	
	/* Step 7: Return desired value */
	ParticleAnalysisReport Report;
	frcParticleAnalysis(ProcessedImage, BestDVIndex, &Report);
	return Report.center_mass_x_normalized;
	
	
}

