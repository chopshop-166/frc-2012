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
		HR.minValue=159; HR.maxValue=255;
		SR.minValue=80 ; SR.maxValue=141;
		LR.minValue=75 ; LR.maxValue=153;
	if(FailCheck(imaqColorThreshold(ProcessedImage, CameraSource, 255, IMAQ_RGB, &HR, &SR, &LR), "ColorThresholdBalls")) return 2; 
	
	/*Step 2: Smooth out bits (clean up)*/
	StructuringElement StructEle;
		StructEle.matrixCols = 3; StructEle.matrixRows = 3; StructEle.hexa = FALSE;
		StructEle.kernel = (int*) malloc(9 * sizeof(int));
		for(int s=0;s<9;s++) StructEle.kernel[s] = 1;
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_ERODE, &StructEle);
	free(StructEle.kernel);
	return 1;
	
	/*Step 3: Filter out pointless particles */
	int IMAQheight;
	int IMAQwidth;
	imaqGetImageSize(ProcessedImage, &IMAQwidth, &IMAQheight);
	ParticleFilterCriteria2 CRIT[3] = {IMAQ_MT_AREA, 400, 76800, FALSE, FALSE};
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	int NP;
	imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT[0], 1, &OPTS, NULL, &NP);
	
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

