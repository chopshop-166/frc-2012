#include "nivision.h"
#include "Vision/MonoImage.h"
#include "Target.h"
#include "BaeUtilities.h"
#include "CameraTask.h"
#include <algorithm>
#include <math.h>

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// These parameters set ellipse finding in the NI imaq (Image Aquisition) library.
// Refer to the CVI Function Reference PDF document installed with LabVIEW for
// additional information.
static EllipseDescriptor ellipseDescriptor = {
										3, 		// minMajorRadius
										200, 	// maxMajorRadius
										3, 		// minMinorRadius
										100		// maxMinorRadius
										};

static CurveOptions curveOptions = {	IMAQ_NORMAL_IMAGE,	// extractionMode
										40, 				// threshold
										IMAQ_NORMAL, 		// filterSize
										25, 				// minLength
										15, 				// rowStepSize 
										15, 				// columnStepSize
										10, 				// maxEndPointGap
										1,					// onlyClosed
										0					// subpixelAccuracy
										};
static ShapeDetectionOptions shapeOptions = {
										IMAQ_GEOMETRIC_MATCH_SHIFT_INVARIANT,	// mode
										NULL,			// angle ranges
										0,				// num angle ranges
										{75, 125},		// scale range
										500				// minMatchScore
};


/**
 * Return the horizontal angle to the target
 */
double Target::GetHorizontalAngle()
{
	double x = m_xPos;
	x = x * 9.0 / m_xMax;
	x = atan2(x, 20.0);
	x = x * 180.0 / PI;
	return x;
}

/**
 * Return the vertical angle to the target
 */
double Target::GetVerticalAngle()
{
	double y = m_yPos;
	y = y * 9.0 * m_xMax;   // is this rignt?
	y = atan2(y, 20.0);
	y = y * 180.0 / PI;
	return y;
}

double Target::GetSize() {
	return 42;
}

/**
 * Compare two targets.
 * Compare the score of two targets for the sort function in C++.
 * @param t1 the first Target
 * @param t2 the second Target
 * @returns (1, 0, or -1) for the scores of t1 > t2, t1 == t2, and t1 < t2
 */
int compareTargets(Target t1, Target t2)
{
	if (t1.m_score > t2.m_score) return 1;
	if (t1.m_score < t2.m_score) return -1;
	return 0;
}

/**
 * Find the best rectangular target in the image.
 * The best target is the one with largest size and best score
 * @param image The image to examine.
 * @returns A target object with the parameters filled in.
 */
vector<Target> Target::FindTargets(HSLImage *image)
{
	wpi_assert(image != NULL);
	int width = image->GetWidth();
	int height = image->GetHeight();

	vector<Target> sortedTargets;

	// get the luminance plane only for the image to make the code
	// insensitive to lighting conditions.
	MonoImage  *luminancePlane = image->GetLuminancePlane();
	vector<EllipseMatch> *results = luminancePlane->DetectEllipses(&ellipseDescriptor, 
																	&curveOptions,
																	&shapeOptions,
																	NULL);
												
	
	/* try
	// cannot convert MonoImage* to Image*
	imaqCast(luminancePlane,luminancePlane,IMAQ_IMAGE_U8,NULL,-1);
	 */				

	/* try 
	 * IMAQ_FUNC int IMAQ_STDCALL imaqArrayToImage(
	 * Image* image, const void* array, int numCols, int numRows);
	 */	
	/* create an image object */
	Image* tmpImage = frcCreateImage(IMAQ_IMAGE_U8);
	if (!tmpImage) {
			int errCode = GetLastVisionError();
			DPRINTF (LOG_INFO,"frcCreateImage failed - errorcode %i", errCode);
	}

	if (!imaqArrayToImage( tmpImage, luminancePlane, 
					luminancePlane->GetWidth(), 
					luminancePlane->GetHeight() ) ) {
		int errCode = GetLastVisionError();
		DPRINTF (LOG_INFO,"imaqArrayToImage failed - errorcode %i", errCode);
		//ERROR_NOT_IMAGE
		char *errString = GetVisionErrorText(errCode);
		DPRINTF (LOG_INFO,"errString= %s", errString);		
	}
	
	// save a file with the the luminance plane
	// cannot convert MonoImage* to Image*
#if SAVE_IMAGES
	//SaveImage("luminance.jpg", luminancePlane);
#endif
	DPRINTF(LOG_INFO,"saving luminance.jpg");
#if SAVE_IMAGES
	//SaveImage("luminance.jpg", tmpImage);
#endif
	delete luminancePlane;
	DPRINTF(LOG_INFO,"found %i possible ellipses", results->size() );
	if (results->size() == 0)
	{
		return sortedTargets;
	}

	// create a list of targets corresponding to each ellipse found
	// in the image.
	for (unsigned i = 0; i < results->size(); i++)
	{
		Target target;
		EllipseMatch e = results->at(i);
		target.m_rawScore = e.score;
		target.m_score = (e.majorRadius * e.minorRadius)
							/ (1001 - e.score)
							/ (height * width) * 100;
		target.m_majorRadius = e.majorRadius / height;
		target.m_minorRadius = e.minorRadius / height;
		//always divide by height so that x and y are same units
		target.m_xPos = (2.0 * e.position.x - width) / height;
		target.m_yPos = (2.0 * e.position.y - height) / height;
		target.m_rotation = e.rotation;
		target.m_xMax = (double)width / height;
		sortedTargets.push_back(target);
	}
	delete results;
	
	// sort the list of targets by score
	// sort the combined targets so the highest scoring one is first
	sort(sortedTargets.begin(), sortedTargets.end(), compareTargets);

	DPRINTF(LOG_INFO,"returning targets" );
	return sortedTargets;
}


/**
 * Print the target.
 * Print information about this target object.
 */
void Target::Print()
{
	printf("Target found: (%f, %f), major: %f, minor: %f\nrotation: %f, score: %f\n\n",
						m_xPos, m_yPos,
						m_majorRadius, m_minorRadius,
						m_rotation, m_score);
}
