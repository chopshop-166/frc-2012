#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Target2.h"
#include "Proxy.h"

#define DPRINTF if(false)dprintf                //debugging info
#define TPRINTF if(true)dprintf                 //testing info


//M's potential debug checker:
static bool FailCheck(int Returned, char* Description)
/*	Description: Prints out errors.
	Syntax to call:
		if(FailCheck(imaqFxn, "Describe function Failed")) return 0;
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


int ProcessMyImage(Image* CameraInput, corner_t DV, float* HeightOfTarget, float* WidthOfTarget)
/*	Description: This function takes an image and analyzes it for the target.
	Return Value:
		0: failed (seeing too many rectangles or none)
		1-4: sees 1-4 rect
*/
{
	//create destination/editable image

	Image* ProcessedImage = frcCreateImage(IMAQ_IMAGE_U8);

	
	/*Step 0: Convert HSL image to usable one
	 int imaqCast(Image* dest, const Image* source, ImageType type, const float* lookup, int shift); */
	//imaqCast
	
	/*Step 1: Color Threshold 
	(Red: 0-255, Green: 174-255,  Blue: 210-255)
	int imaqColorThreshold(Image* dest, const Image* source, int replaceValue, ColorMode mode, const Range* plane1Range, const Range* plane2Range, const Range* plane3Range); */
	/* Setup Threshold Values */
	Range RR, GR, BR;
		RR.minValue=0  ; RR.maxValue=255;
		GR.minValue=174; GR.maxValue=255;
		BR.minValue=210; BR.maxValue=255;
	FailCheck(imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_RGB, &RR, &GR, &BR), "Color Threshold Failed");

	/*Step 2: Basic Morphology Dilation
	int imaqMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement); */
	/* Setup for Morphology*/
		StructuringElement StructEle;
		StructEle.matrixCols = 3; StructEle.matrixRows = 3; StructEle.hexa = FALSE;
		StructEle.kernel = (int*) malloc(9 * sizeof(int));
		for(int s=0;s<9;s++) StructEle.kernel[s] = 1;
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	free(StructEle.kernel);

	/*Step 3: Fill Holes
	int imaqFillHoles(Image* dest, const Image* source, int connectivity8); */
	imaqFillHoles(ProcessedImage, ProcessedImage, TRUE); 

	/*Step 4: Particle Filter (Area: min.5%, max100% of image)
	int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
	/* Setup for Particle Filter */
	ParticleFilterCriteria2 CRIT;
		CRIT.parameter=IMAQ_MT_AREA_BY_IMAGE_AREA;
		CRIT.lower=0.5; CRIT.upper=100;
		CRIT.calibrated=FALSE; CRIT.exclude=FALSE;
	ParticleFilterOptions2 OPTS;
		OPTS.rejectMatches=FALSE; OPTS.rejectBorder=TRUE;
		OPTS.fillHoles=FALSE; OPTS.connectivity8=TRUE;
	int NP;
	imaqParticleFilter4(ProcessedImage, ProcessedImage, &CRIT, 1, &OPTS, NULL, &NP); 

	/*Step 5: Convert to 16-bit image, max contrast
	int imaqLookup(Image* dest, const Image* source, const short* table, const Image* mask); */
	/* Setup for Lookup Table */
	short TBL[256];
		TBL[0]=0;
		for(int i=1;i<257;i++) TBL[i]=255;
	imaqLookup(ProcessedImage, ProcessedImage, &TBL[0], NULL); 

	/* Step 6: Find Rectangles
	RectangleMatch* imaqDetectRectangles(const Image* image, const RectangleDescriptor* rectangleDescriptor, const CurveOptions* curveOptions, const ShapeDetectionOptions* shapeDetectionOptions, const ROI* roi, int* numMatchesReturned); */
	/* Setup for Rectangle Detection */
		RectangleDescriptor RCDS;
			RCDS.minWidth =0;RCDS.maxWidth =1000000;
			RCDS.minHeight=0;RCDS.maxHeight=1000000;
		CurveOptions CVOP;
			CVOP.extractionMode=IMAQ_UNIFORM_REGIONS;
			CVOP.threshold=1;
			CVOP.filterSize=IMAQ_NORMAL;
			CVOP.minLength=10;
			CVOP.rowStepSize=10;
			CVOP.columnStepSize=10;
			CVOP.maxEndPointGap=10;
			CVOP.onlyClosed=FALSE;
			CVOP.subpixelAccuracy=TRUE;//ignored
		ShapeDetectionOptions SHDTOP;
			SHDTOP.mode=IMAQ_GEOMETRIC_MATCH_SCALE_INVARIANT;
				RangeFloat ARF; ARF.minValue=-8; ARF.maxValue=8;
			SHDTOP.angleRanges=&ARF;
			SHDTOP.numAngleRanges=1;
				RangeFloat SRF; SRF.minValue=.25; SRF.maxValue=200;
			SHDTOP.scaleRange=SRF;
			SHDTOP.minMatchScore=400;
	int NumRect; //number of rectangles seen
	RectangleMatch* RectMPtr=imaqDetectRectangles(ProcessedImage, &RCDS, &CVOP, &SHDTOP, NULL, &NumRect);
	if(NumRect==0 || NumRect>4) return 0; 

	/* Step 7: Find desired Rectangle */ 
	/* Find Desired Value (DV) rectange's number logic */
		int BestDVRectPtr;
		float BestDVSoFar=0;
		switch (DV)//Find the rectangle with the desired value
		{
			case TOP_MOST:
				for (int i = 0; i < NumRect; i++) 
				{
					if ((RectMPtr+i)->corner[0].y < BestDVSoFar)
					{ BestDVSoFar = (RectMPtr+i)->corner[0].y; BestDVRectPtr = i; }
				}
			break;
			case LEFT_MOST:
				for (int i = 0; i < NumRect; i++) 
				{
					if ((RectMPtr+i)->corner[0].x < BestDVSoFar)
					{ BestDVSoFar = (RectMPtr+i)->corner[0].x; BestDVRectPtr = i; }
				}
			break;
			case RIGHT_MOST:
				for (int i = 0; i < NumRect; i++) 
				{
					if ((RectMPtr+i)->corner[0].x > BestDVSoFar)
					{ BestDVSoFar = (RectMPtr+i)->corner[0].x; BestDVRectPtr = i; }
				}
			case BOTTOM_MOST:
				for (int i = 0; i < NumRect; i++) 
				{
					if ((RectMPtr+i)->corner[0].y > BestDVSoFar)
					{ BestDVSoFar = (RectMPtr+i)->corner[0].y; BestDVRectPtr = i; }
				}
			break;
			default:
				printf("\n\nSome fool put a bad value into M's image processing program!!\n\n");
			break;
		}
		RectangleMatch* TargetRect = RectMPtr + BestDVRectPtr;
	
	
	/* Step 8: Take desired measurements and return number of rectangles seen*/
	/* Take measurements, return */
	*HeightOfTarget = ((TargetRect->corner[0].y + TargetRect->corner[1].y)/2);
	*WidthOfTarget = TargetRect->corner[0].x - TargetRect->corner[1].x;
	TPRINTF(LOG_INFO, "TargetRect:\t1: %f, %f\t2: %f, %f\t3: %f, %f\t4: %f, %f\r", 
			(double) TargetRect->corner[0].x,
			(double) TargetRect->corner[0].y,
			(double) TargetRect->corner[1].x,
			(double) TargetRect->corner[1].y,
			(double) TargetRect->corner[2].x,
			(double) TargetRect->corner[2].y,
			(double) TargetRect->corner[3].x,
			(double) TargetRect->corner[3].y
	);
	return NumRect;
	
	
	imaqDispose(RectMPtr);

}
