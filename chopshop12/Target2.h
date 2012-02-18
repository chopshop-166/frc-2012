#ifndef _TARGET2_H
#define _TARGET2_H
typedef enum {
	TOP_MOST=0, 
	LEFT_MOST, 
	RIGHT_MOST, 
	BOTTOM_MOST, 
	UNINITIALIZED} corner_t;

extern int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep, int btnInput);
/* ProcessMyImage takes the camera image and an array of 4 ParticleAnalysisReports.
 * The button input isn't used here.
 * Upon success, the number of targets seen is returned. */
extern bool FailCheck(int Returned, char* Description);
/* FailCheck takes an NI Vision Function and a description. 
 * The description is returned upon failure. 
 * Upon an NI Vision failure, 'true' is returned. */
#endif //_TARGET2_H

