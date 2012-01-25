typedef enum {
	UNINITIALIZED=0,
	TOP_MOST, 
	LEFT_MOST, 
	RIGHT_MOST,
	BOTTOM_MOST
} corner_t;

extern int ProcessMyImage(Image* CameraInput, corner_t DV, float* HeightOfTarget, float* WidthOfTarget);
