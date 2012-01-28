#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "nivision.h"
#include "VisionAPI.h"
#include "TrackAPI.h"
#include "stdlib.h"
#include "Target2.h"
#include "Proxy.h"

#define DPRINTF if(true)dprintf					//debugging info
#define TPRINTF if(true)dprintf					//testing info
#define I_SEE_4_TARGETS (false)					
/* If you have 4 vision targets to look at, set this to true
 * to enable target selection. Note that the image must contain
 * at least 4 particles for this to work.*/


//M's potential debug checker:
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


int ProcessMyImage(Image* CameraInput, corner_t DV, float* HeightOfTarget, float* WidthOfTarget)
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
	Range RR, GR, BR;
		RR.minValue=0  ; RR.maxValue=255;
		GR.minValue=0  ; GR.maxValue=255;
		BR.minValue=140; BR.maxValue=255;
	int thresholdcheck;
	thresholdcheck=imaqColorThreshold(ProcessedImage, CameraInput, 255, IMAQ_HSL, &RR, &GR, &BR);
	if(FailCheck(thresholdcheck, "Color Threshold Failed %i")) {return 0; } else {DPRINTF(LOG_INFO, "Thresholded");}
	
	Countup(ProcessedImage);
	/*Step 2: Basic Morphology Dilation
	int imaqMorphology(Image* dest, Image* source, MorphologyMethod method, const StructuringElement* structuringElement); */
	/* Setup for Morphology*/
		StructuringElement StructEle;
		StructEle.matrixCols = 3; StructEle.matrixRows = 3; StructEle.hexa = FALSE;
		StructEle.kernel = (int*) malloc(9 * sizeof(int));
		for(int s=0;s<9;s++) StructEle.kernel[s] = 1;
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle);
	if (FailCheck(imaqMorphology(ProcessedImage, ProcessedImage, IMAQ_DILATE, &StructEle), "Dilation 1 %f")) {return 0; } else {DPRINTF(LOG_INFO, "Dilated");}
	free(StructEle.kernel);

	Countup(ProcessedImage);
	/*Step 3: Fill Holes
	int imaqFillHoles(Image* dest, const Image* source, int connectivity8); */
	if(FailCheck(imaqFillHoles(ProcessedImage, ProcessedImage, TRUE), "Fill Holes %i")) {return 0; } else {DPRINTF(LOG_INFO, "Holes Filled");}

	/*Step 4: Particle Filter (Area: min.5%, max100% of image)
	int imaqParticleFilter4(Image* dest, Image* source, const ParticleFilterCriteria2* criteria, int criteriaCount, const ParticleFilterOptions2* options, const ROI* roi, int* numParticles); */
	/* Setup for Particle Filter */
	ParticleFilterCriteria2 CRIT;
		CRIT.parameter=IMAQ_MT_AREA_BY_IMAGE_AREA;
		CRIT.lower=1; CRIT.upper=100;
		CRIT.calibrated=FALSE; CRIT.exclude=FALSE;
	ParticleFilterOptions2 OPTS;
		OPTS.rejectMatches=FALSE; OPTS.rejectBorder=TRUE;
		OPTS.fillHoles=FALSE; OPTS.connectivity8=TRUE;
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

	/* Step 6: Get particle */
	int DesiredParticleIndex;
	int NumPart=Countup(ProcessedImage);
#if (I_SEE_4_TARGETS)
	/* Step ALT 6: Particle analysis */
	//Step 6.1: find the index numbers of the 4 targets
	int index[4];
	if (NumPart>4)
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
		DPRINTF(LOG_INFO, "Cannot see enough!")
		return 0;
	}
	//Step 6.2: Select the desired particle and eliminate all others
	int BestDVIndex;
	double BestDV=0;
	switch (DV)//Find the rectangle with the desired value
	{
		case TOP_MOST:
			for (int i = 0; i < 4; i++) 
			{
				double testmeasure;
				imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_TOP, &testmeasure);
				if (testmeasure < BestDV)
				{ BestDV=testmeasure; BestDVIndex = i; }
			}
		break;
		case LEFT_MOST:
			for (int i = 0; i < 4; i++) 
				{
					double testmeasure;
					imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_RIGHT, &testmeasure);
					if (testmeasure < BestDV)
					{ BestDV=testmeasure; BestDVIndex = i; }
				}
		break;
		case RIGHT_MOST:
			for (int i = 0; i < 4; i++) 
				{
					double testmeasure;
					imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_RIGHT, &testmeasure);
					if (testmeasure > BestDV)
					{ BestDV=testmeasure; BestDVIndex = i; }
				}
		case BOTTOM_MOST:
			for (int i = 0; i < 4; i++) 
				{
					double testmeasure;
					imaqMeasureParticle(ProcessedImage, index[i],  0, IMAQ_MT_BOUNDING_RECT_TOP, &testmeasure);
					if (testmeasure > BestDV)
					{ BestDV=testmeasure; BestDVIndex = i; }
				}
		break;
		default:
			printf("\n\nSome fool put a bad value into M's image processing program!!\n\n");
		break;
	}
	//Step 6.3: Magically pick out the one particle we want
	double Xval;
	double Yval;
	imaqMeasureParticle(ProcessedImage, index[BestDVIndex], 0, IMAQ_MT_CENTER_OF_MASS_X, &Xval); 
	imaqMeasureParticle(ProcessedImage, index[BestDVIndex], 0, IMAQ_MT_CENTER_OF_MASS_Y, &Yval); 
	Point Coords;
	Coords.x = (int) Xval;
	Coords.y = (int) Yval;
	if(FailCheck(imaqMagicWand(ProcessedImage, ProcessedImage, Coords, 5, TRUE, 255), "Magic Wand %i")){return 0;} else {DPRINTF(LOG_INFO, "MagicWand!");}
	Countup(ProcessedImage);
	DesiredParticleIndex=0;
#endif

#if (!I_SEE_4_TARGETS)
	//DesiredParticleIndex
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
#endif	
	
/* Step 7: Find best particle and analyze */
	ParticleAnalysisReport PAR;
	frcParticleAnalysis(ProcessedImage, DesiredParticleIndex, &PAR);
	DPRINTF(LOG_INFO, "Particle Analysis: %f", PAR.center_mass_x_normalized);
/*A Particle Analysis Report:
		int 	imageHeight;
		int 	imageWidth;
		double 	imageTimestamp;				
		int		particleIndex;
		int 	center_mass_x;  			// MeasurementType: IMAQ_MT_CENTER_OF_MASS_X 
		int 	center_mass_y;  			// MeasurementType: IMAQ_MT_CENTER_OF_MASS_Y 
		double 	center_mass_x_normalized;  	//Center of mass x value normalized to -1.0 to +1.0 range
		double 	center_mass_y_normalized;  	//Center of mass y value normalized to -1.0 to +1.0 range
		double 	particleArea;				// MeasurementType: IMAQ_MT_AREA
		Rect 	boundingRect;				// left/top/width/height
			int top;    //Location of the top edge of the rectangle.
		    int left;   //Location of the left edge of the rectangle.
		    int height; //Height of the rectangle.
		    int width;  //Width of the rectangle.
		double 	particleToImagePercent;		// MeasurementType: IMAQ_MT_AREA_BY_IMAGE_AREA
		double 	particleQuality;			// MeasurementType: IMAQ_MT_AREA_BY_PARTICLE_AND_HOLES_AREA
		//particleQuality: Percentage of the particle Area in relation to its Particle and Holes Area
*/
	return Countup(ProcessedImage);
	
	
	
	
	
	
	
	
	

	
#if 0
	/* Step 6: Find Rectangles
	RectangleMatch* imaqDetectRectangles(const Image* image, const RectangleDescriptor* rectangleDescriptor, const CurveOptions* curveOptions, const ShapeDetectionOptions* shapeDetectionOptions, const ROI* roi, int* numMatchesReturned); */
	/* Setup for Rectangle Detection */
		RectangleDescriptor RCDS;
			RCDS.minWidth =0;RCDS.maxWidth =100000000;
			RCDS.minHeight=0;RCDS.maxHeight=100000000;
		CurveOptions CVOP;
			CVOP.extractionMode=IMAQ_UNIFORM_REGIONS;
			CVOP.threshold=1;
			CVOP.filterSize=IMAQ_NORMAL;
			CVOP.minLength=5;
			CVOP.rowStepSize=10;
			CVOP.columnStepSize=10;
			CVOP.maxEndPointGap=10;
			CVOP.onlyClosed=FALSE;
			CVOP.subpixelAccuracy=TRUE;//ignored
		ShapeDetectionOptions SHDTOP;
			SHDTOP.mode=IMAQ_GEOMETRIC_MATCH_SCALE_INVARIANT;
				RangeFloat ARF; ARF.minValue=-20; ARF.maxValue=20;
			SHDTOP.angleRanges=&ARF;
			SHDTOP.numAngleRanges=1;
				RangeFloat SRF; SRF.minValue=.25; SRF.maxValue=200;
			SHDTOP.scaleRange=SRF;
			SHDTOP.minMatchScore=200;
	int NumRect; //number of rectangles seen
	RectangleMatch* RectMPtr=imaqDetectRectangles(ProcessedImage, &RCDS, &CVOP, &SHDTOP, NULL, &NumRect);
	DPRINTF(LOG_INFO, "Rectangles in fxn: %i", NumRect);
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
	
	
	/* Step 8: Take desired measurements and return number of rectangles seen
	* Take measurements, return */
	*HeightOfTarget = ((TargetRect->corner[0].y + TargetRect->corner[1].y)/2);
	*WidthOfTarget = TargetRect->corner[0].x - TargetRect->corner[1].x;
	TPRINTF(LOG_INFO, "TargetRect:\t1: %f, %f\n", 
			(double) TargetRect->corner[0].x,
			(double) TargetRect->corner[0].y
	);*/
	return NumRect;
	imaqDispose(RectMPtr);
#endif

}
