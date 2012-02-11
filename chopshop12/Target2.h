#ifndef _TARGET2_H
#define _TARGET2_H
typedef enum {
	TOP_MOST=0, 
	LEFT_MOST, 
	RIGHT_MOST, 
	BOTTOM_MOST, 
	UNINITIALIZED} corner_t;

extern int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep, int btnInput);
#endif //_TARGET2_H

