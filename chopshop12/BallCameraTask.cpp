/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: BallCameraTask.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: This task initializes the camera and calls the 2011
*           targeting code. It offers a static fuction that any task can call 
*                   BallCameraTask::TakeSnapshot(char* imageName))
*			to save a picture to the cRIO.
*           Logged target data: size, location, and score (a
*                     quality metric)
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Robot.h"
#include "BallCameraTask.h"
#include "nivision.h"
#include "TargetB.h"
#include "TargetingInfo.h"




// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

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
class BallCameraLog : public MemoryLog
{
public:
	BallCameraLog() : MemoryLog(
			sizeof(struct abuf), CAMERA_CYCLE_TIME, "camera",
			"Elapsed Time,H-Angle, V-Angle, Size,\n" 
			) {
		return;
	};
	~BallCameraLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file

	unsigned int PutOne(double,double,double);     // Log the values needed-add in arguments
};

// Write one buffer into memory
unsigned int BallCameraLog::PutOne(double hAngle,double vAngle,double size)
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
unsigned int BallCameraLog::DumpBuffer(char *nptr, FILE *ofile)
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

BallCameraTask *BallCameraTask::myHandle = NULL;

// task constructor
BallCameraTask::BallCameraTask(void):camera(AxisCamera::GetInstance("10.1.66.11"))
{
	myHandle = this;
	printf("This is the BallCamera constructor\n");
	/* allow writing to vxWorks target */
	Priv_SetWriteFileAllowed(1);   
		
	this->MyTaskIsEssential=0;
	
	SetDebugFlag ( DEBUG_SCREEN_ONLY  );
	camera.WriteResolution(AxisCamera::kResolution_320x240);
	camera.WriteBrightness(10);
	int fps = camera.GetMaxFPS();
	Start((char *)"BallCameraTask", CAMERA_CYCLE_TIME);

	DPRINTF(LOG_INFO,"BallCameraTask FPS=%i task cycle time=%i",fps,CAMERA_CYCLE_TIME);

	printf("End of BallCamera constructor\n");
	return;
};
	
// task destructor
BallCameraTask::~BallCameraTask(void)
{
	return;
};
	
// Main function of the task
int BallCameraTask::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	bool found = false;
	
	// Register our logger
	BallCameraLog sl;                   // log
	
	// Register the proxy
	proxy = Proxy::getInstance();
	DPRINTF(LOG_INFO,"BallCameraTask got proxy");
	
	//Proxy values
	proxy->add("BallCameraX");
	
	
	// Let the world know we're in
	DPRINTF(LOG_INFO,"In the 166 BallCamera task\n");
	
	WaitForGoAhead(); // THIS IS VERY IMPORTANT

	
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	DPRINTF(LOG_INFO,"BallCameraTask registered logger");
	lHandle->DriverStationDisplay("BallCamera Task...");
	
	DPRINTF(LOG_INFO,"BallCameraTask informed DS");
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {				
		// Wait for our next lap
		WaitForNextLoop();
		
		/* Look for target */
		found = BallCameraTask::FindTargets();
        found = false;
	    // Logging values if a valid target found
		if (found) {
			sl.PutOne(targetHAngle,targetVAngle,targetSize);
		}
		
		// JUST FOR DEBUGGING - give us time to look at the screen
		// REMOVE THIS WAIT to go operational!
		Wait(3.0);
	}
	return (0);
	
};


/** 
 * Call the targeting code that looks for rectangular objects.
 * TODO: change to Rectangular shapes
 * @return bool success code
 */
bool BallCameraTask::FindTargets() {

	lHandle->DriverStationDisplay("ProcessImage:%0.6f",GetTime());

	// get the camera image
    HSLImage* image1 = camera.GetImage();   
    Image* image = image1->GetImaqImage();
		
    //Image Processing 2
    proxy->set("BallCameraX", (float) ProcessMyImageForBalls(image));
    
	//delete image;
	imaqDispose(image);
    return 1;
}

