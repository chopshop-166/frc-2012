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

int ProcessMyImageB(Image* CameraSource)
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
	free(StructEle.kernel);
	return 1;
	
	/*Step 3: */
}

