typedef enum {
	TOP_MOST=0, 
	LEFT_MOST, 
	RIGHT_MOST, 
	BOTTOM_MOST, 
	UNINITIALIZED 
} corner_t;

extern int ProcessMyImage(Image* CameraInput, ParticleAnalysisReport* ParticleRep);
