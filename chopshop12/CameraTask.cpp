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
#include "nivision.h"
#include "Target2.h"
#include "TargetingInfo.h"
#include "Turret.h"



// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf
#define TPRINTF if(false)dprintf
#define MPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	double normalizedCenterX;
	int numParticles;
};

//  Memory Log
class CameraLog : public MemoryLog
{
public:
	CameraLog() : MemoryLog(
			sizeof(struct abuf), CAMERA_CYCLE_TIME, "camera",
			"Elapsed Time,normalizedCenterX,numParticles\n" 
			) {
		return;
	};
	~CameraLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file

	unsigned int PutOne(double normalizedCenterX,int numParticles);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int CameraLog::PutOne(double normalizedCenterX,int numParticles)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here

		ob->normalizedCenterX = normalizedCenterX;
		ob->numParticles = numParticles;
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
	fprintf(ofile, "%4.5f, %3.3f, %i \n",
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			// Values to log
			ab->normalizedCenterX, 
			ab-> numParticles	
	);
	
	// Done
	return (sizeof(struct abuf));
}

CameraTask *CameraTask::myHandle = NULL;

// task constructor
CameraTask::CameraTask(void):camera(AxisCamera::GetInstance("10.1.66.12"))
{
	myHandle = this;
	printf("This is the Camera constructor\n");
	/* allow writing to vxWorks target */
	Priv_SetWriteFileAllowed(1);   
		
	this->MyTaskIsEssential=0;
	
	SetDebugFlag ( DEBUG_SCREEN_ONLY  );
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteBrightness(10);
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
	proxy->add("WidthOfTarget");*/
	proxy->add("CameraX");
	proxy->add("turret_angle");
	proxy->add("initial_velocity");
	
	
	// Let the world know we're in
	DPRINTF(LOG_INFO,"In the 166 Camera task\n");
	
	WaitForGoAhead(); // THIS IS VERY IMPORTANT

	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	DPRINTF(LOG_INFO,"CameraTask registered logger");
	lHandle->DriverStationDisplay("Camera Task...");
	
	DPRINTF(LOG_INFO,"CameraTask informed DS");
	DPRINTF(LOG_INFO,"About to...");
	TakeSnapshot("FIRSTcRIOimage.jpg");
	DPRINTF(LOG_INFO,"...take a picture!");
	

	double normalizedCenterX;
	int numParticles;
	int loopcounter=0;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {				
		// Wait for our next lap
		WaitForNextLoop();
#if SAVE_IMAGES
		/* Store a picture to cRIO */
		TakeSnapshot("cRIOimage.jpg");
#endif
		
		/* Look for target */
		if(((loopcounter++) %10)==0)
		{
			dprintf(LOG_INFO, "NumParticles=%i", numParticles);
		}
		if(proxy->get(TURRET_BTN_AUTO))
		{
			found = CameraTask::FindTargets(&normalizedCenterX, &numParticles);
		}
		
		sl.PutOne(normalizedCenterX, numParticles);
        found = false;
	    // Logging is done in FindTargets();
		
		
		// JUST FOR DEBUGGING - give us time to look at the screen
		// REMOVE THIS WAIT to go operational!
		//Wait(1);
	}
	return (0);
	
};


/** 
 * Call the targeting code that looks for rectangular objects.
 * TODO: change to Rectangular shapes
 * @return bool success code
 */
bool CameraTask::FindTargets(double* normalizedCenterX, int* numParticles) {

	

	// get the camera image. The image that is created is
	//	created with a "new" function so must be deleted.
    HSLImage* image1 = camera.GetImage();   

	// Get a pointer to the internal IMAQ image that is
	// in image1. This DOES NOT allocate any memory!
    Image* image = image1->GetImaqImage();

	int BTN_INPUT = 0;
		
	ParticleAnalysisReport ParticleReport[4];
	*numParticles = ProcessMyImage(image, &ParticleReport[0], BTN_INPUT);
	MPRINTF(LOG_INFO, "RETURNED: %i", *numParticles);
	if(*numParticles>0)
	{
#define SELECTED_TARGET (TOP_MOST)
		*normalizedCenterX = ParticleReport[SELECTED_TARGET].center_mass_x_normalized;
		proxy->set("CameraX", (float) ParticleReport[SELECTED_TARGET].center_mass_x_normalized);
		printf("CameraX= %f\n", (float) ParticleReport[SELECTED_TARGET].center_mass_x_normalized);
	}
	else
	{
		*normalizedCenterX = 2;
		proxy->set("CameraX", (float) 2);
		printf("CameraX= %f\n", 2.0);
	}
	lHandle->DriverStationDisplay("ProcessImage:%0.6f",proxy->get("CameraX"));
	
	Ballistics(&ParticleReport[0], BTN_INPUT);
	
	
	delete image1;
    return 1;
}

/**
 * Take a picture and store it to the cRIO in the specified path
 * Any task should be able to call this to take and save a snapshot
 */
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
		} 
	}
	else {
			DPRINTF (LOG_INFO,"Image is stale");	
	} // fresh
	frcDispose(cameraImage);
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


