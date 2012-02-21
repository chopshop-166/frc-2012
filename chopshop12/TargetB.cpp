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
	
	/*Step 1: RGB Color plane extraction*/
	Range HR, SR, LR;
		HR.minValue=150; HR.maxValue=215;
		SR.minValue=80 ; SR.maxValue=145;
		LR.minValue=30 ; LR.maxValue=130;
	imaqColorThreshold(ProcessedImage, CameraSource, 255, IMAQ_RGB, &HR, &SR, &LR); 
	
	/*Step 2: Filter out pointless particles */
	ParticleFilterCriteria2 CRIT = {IMAQ_MT_AREA, 100, 76800, FALSE, FALSE};
	const ParticleFilterOptions2 OPTS = {FALSE, FALSE, FALSE, TRUE};
	int NP;
	imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT, 1, &OPTS, NULL, &NP);
	
	/*Step 3: Smooth out bits (clean up)*/
	StructuringElement StructEle;
		StructEle.matrixCols = 3; StructEle.matrixRows = 3; StructEle.hexa = FALSE;
		StructEle.kernel = (int*) malloc(9 * sizeof(int));
		for(int s=0;s<9;s++) StructEle.kernel[s] = 1;
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	free(StructEle.kernel);
	
	/*Step 4: Find best particle */
	int numParticles;
	imaqCountParticles(ProcessedImage, TRUE, &numParticles);
	if(numParticles==0)
	{
		frcDispose(ProcessedImage);
		return 2;
	}
	int BestDVIndex;
	double BestDV=0;
	double testmeasure;
	for (int i = 0; i < numParticles; i++) 
	{
		imaqMeasureParticle(ProcessedImage, i,  0, IMAQ_MT_CENTER_OF_MASS_Y, &testmeasure);
		if (testmeasure > BestDV)
		{ BestDV=testmeasure; BestDVIndex = i; }
	}
	
	/* Step 5: Return desired value */
	ParticleAnalysisReport Report;
	frcParticleAnalysis(ProcessedImage, BestDVIndex, &Report);
	frcDispose(ProcessedImage);
	return Report.center_mass_x_normalized;
}

