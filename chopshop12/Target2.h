typedef enum {
	UNINITIALIZED=0,
	TOP_MOST, 
	LEFT_MOST, 
	RIGHT_MOST,
	BOTTOM_MOST
} corner_t;

extern int ProcessImage(Image* CameraInput, corner_t DV/*, outputs*/);
