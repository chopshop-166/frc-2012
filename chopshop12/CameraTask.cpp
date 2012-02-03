/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: CameraTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: This task initializes the camera and calls the 2011
*           targeting code. It offers a static fuction that any task can call 
*                   CameraTask::TakeSnapshot(char* imageName))
*			to save a picture to the cRIO.
*           Logged target data: size, location, and score (a
*                     quality metric)
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Robot.h"
#include "CameraTask.h"
#include "Target.h"
#include "nivision.h"
#include "Target2.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(true)dprintf
#define TPRINTF if(false)dprintf
#define M_METHOD (0)

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	double targetHAngle;
	double targetVAngle;
	double targetSize;	
};

//  Memory Log
class CameraLog : public MemoryLog
{
public:
	CameraLog() : MemoryLog(
			sizeof(struct abuf), CAMERA_CYCLE_TIME, "camera",
			"Elapsed Time,H-Angle, V-Angle, Size,\n" 
			) {
		return;
	};
	~CameraLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file

	unsigned int PutOne(double,double,double);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int CameraLog::PutOne(double hAngle,double vAngle,double size)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here

		ob-> targetHAngle = hAngle;
		ob-> targetVAngle = vAngle;
		ob-> targetSize = size;	
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int CameraLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%4.5f, %3.3f, %3.3f, %4.4f\n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			// Values to log
			ab-> targetHAngle, ab-> targetVAngle, ab-> targetSize		
	);
	
	// Done
	return (sizeof(struct abuf));
}

CameraTask *CameraTask::myHandle = NULL;

// task constructor
CameraTask::CameraTask(void):camera(AxisCamera::GetInstance("10.1.66.11"))
{
	myHandle = this;
	printf("This is the Camera constructor\n");
	/* allow writing to vxWorks target */
	Priv_SetWriteFileAllowed(1);   
		
	this->MyTaskIsEssential=0;
	
	SetDebugFlag ( DEBUG_SCREEN_ONLY  );
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteBrightness(30);
	int fps = camera.GetMaxFPS();
	Start((char *)"CameraTask", CAMERA_CYCLE_TIME);

	DPRINTF(LOG_INFO,"CameraTask FPS=%i task cycle time=%i",fps,CAMERA_CYCLE_TIME);

	printf("End of Camera constructor\n");
	return;
};
	
// task destructor
CameraTask::~CameraTask(void)
{
	return;
};
	
// Main function of the task
int CameraTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	bool found = false;
	
	// Register our logger
	CameraLog sl;                   // log
	
	// Register the proxy
	proxy = Proxy::getInstance();
	DPRINTF(LOG_INFO,"CameraTask got proxy");
	
	/* Changed to a particle analysis report
	proxy->add("WidthOfTarget");
	proxy->add("HeightOfTarget");	
	*/
	
	// Let the world know we're in
	DPRINTF(LOG_INFO,"In the 166 Camera task\n");
	
	WaitForGoAhead(); // THIS IS VERY IMPORTANT

	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	DPRINTF(LOG_INFO,"CameraTask registered logger");
	lHandle->DriverStationDisplay("Camera Task...");
	
	DPRINTF(LOG_INFO,"CameraTask informed DS");
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {				
		// Wait for our next lap
		WaitForNextLoop();
		
		/* Store a picture to cRIO */
		TakeSnapshot("cRIOimage.jpg");
		
		/* Look for target */
		found = FindTargets();
        found = false;
	    // Logging values if a valid target found
		if (found) {
			sl.PutOne(targetHAngle,targetVAngle,targetSize);
		}
		
		// JUST FOR DEBUGGING - give us time to look at the screen
		// REMOVE THIS WAIT to go operational!
		Wait(10.0);
	}
	return (0);
	
};


/** 
 * Call the targeting code that looks for rectangular objects.
 * TODO: change to Rectangular shapes
 * @return bool success code
 */
bool CameraTask::FindTargets() {

	lHandle->DriverStationDisplay("ProcessImage:%0.6f",GetTime());

	// get the camera image
    HSLImage* image1 = camera.GetImage();   
    Image* image = image1->GetImaqImage();
#if M_METHOD
	// find FRC targets in the image
	vector<Target> targets = Target::FindTargets(image);
	
		if (targets.size()) {
			DPRINTF(LOG_DEBUG, "targetImage SCORE = %f", targets[0].m_score);
		}	
#endif
#if M_METHOD
		if (targets.size() == 0) {
			// no targets found.
			DPRINTF(LOG_DEBUG, "No target found\n\n");
			return false;			
		}
		else if (targets[0].m_score < MINIMUM_SCORE) {
			// no good enough targets found
			DPRINTF(LOG_DEBUG, "No valid targets foxxxund, best score: %f ", 
						targets[0].m_score);
			return false;			
		}
		else {
			// We have some targets.
			// set the new PID heading setpoint to the first target in the list
			targetHAngle = targets[0].GetHorizontalAngle();
			targetVAngle = targets[0].GetVerticalAngle();
			targetSize = targets[0].GetSize();

			// send dashboard data for target tracking
			DPRINTF(LOG_DEBUG, "Target found %f ", targets[0].m_score);
			DPRINTF(LOG_DEBUG, "H: %3.0f  V: %3.0f  SIZE: %3.3f ", 
					targetHAngle, targetVAngle, targetSize);
//			targets[0].Print();
		}
		return true;
#endif
		
		ParticleAnalysisReport ParticleReport[4];
		dprintf(LOG_INFO, "RETURNED: %i", ProcessMyImage(image, &ParticleReport[0]));
		/*
		proxy->set("HeightOfTarget", (float) HeightOfTarget);
	    proxy->set("WidthOfTarget", WidthOfTarget);
	    */
		
		/*A Particle Analysis Report contains:
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
		
		//delete image;
		imaqDispose(image);
	    return 1;
}

/**
 * Take a picture and store it to the cRIO in the specified path
 * Any task should be able to call this to take and save a snapshot
 */
#if SAVE_IMAGES
void CameraTask::TakeSnapshot(char* imageName)  {
	
	myHandle->lHandle->DriverStationDisplay("storing %s",imageName);
	//DPRINTF(LOG_DEBUG, "taking a SNAPSHOT ");
	
	Image* cameraImage = frcCreateImage(IMAQ_IMAGE_HSL);
	if (!cameraImage) {
		DPRINTF (LOG_INFO,"frcCreateImage failed - errorcode %i",GetLastVisionError()); 
	    return;
	}
	/* If there is an unacquired image to get, acquire it */
	if ( myHandle->camera.IsFreshImage() ) {
		if ( !myHandle->camera.GetImage(cameraImage) ) {
			DPRINTF (LOG_INFO,"\nImage Acquisition from camera failed %i", GetLastVisionError());
		} else { 	
			SaveImage(imageName, cameraImage);
		  	// always dispose of image objects when done
		  	frcDispose(cameraImage);
		} 
	}
	else {
			DPRINTF (LOG_INFO,"Image is stale");	
	} // fresh
};

/**
 * Store an Image to the cRIO in the specified path
 */
void SaveImage(char* imageName, Image* image)  {	
	DPRINTF (LOG_DEBUG, "writing %s", imageName);
	if (!frcWriteImage(image, imageName) ) { 
		int errCode = GetLastVisionError();
		DPRINTF (LOG_INFO,"frcWriteImage failed - errorcode %i", errCode);
		char *errString = GetVisionErrorText(errCode);
		DPRINTF (LOG_INFO,"errString= %s", errString);
	} 
};
#endif

