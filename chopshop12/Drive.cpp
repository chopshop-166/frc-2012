/*******************************************************************************
*  Project   		: Framework
*  File Name  		: TaskDrive.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Drive source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "Drive" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskDrive" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "Drive.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
// <<CHANGEME>>
class DriveLog : public MemoryLog
{
public:
	DriveLog() : MemoryLog(
			sizeof(struct abuf), DRIVE_CYCLE_TIME, "Drive",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~DriveLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int DriveLog::PutOne(void)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		// Add any values to be logged here
		// <<CHANGEME>>
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int DriveLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.))
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
Drive166::Drive166(void):
	FrontLeft(DRIVE_FRONT_LEFT), 
	FrontRight(DRIVE_FRONT_RIGHT), 
	RearLeft(DRIVE_REAR_LEFT), 
	RearRight(DRIVE_REAR_RIGHT)
	
{
	Start((char *)"166DriveTask", DRIVE_CYCLE_TIME);
	// Register the proxy
	proxy = Proxy::getInstance();
	opposite = 0;
	return;
};
	
// task destructor
Drive166::~Drive166(void)
{
	return;
};
	
// Main function of the task
int Drive166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	DriveLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Drive task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
    // General main loop (while in Autonomous or Tele mode)
	proxy->TrackNewpress(DRIVE_OPPOSITE_BUTTON_LEFT);
	proxy->TrackNewpress(DRIVE_OPPOSITE_BUTTON_RIGHT);
	while (true) {

		DriveSpeed1 = proxy->get(DRIVE_1_JOYSTICK_Y);
		DriveSpeed2 = proxy->get(DRIVE_2_JOYSTICK_Y);
		
		//if button 2 is pressed, inverts controls, input * -1
		if(proxy->get(DRIVE_OPPOSITE_BUTTON_N_LEFT) || proxy->get(DRIVE_OPPOSITE_BUTTON_N_RIGHT))
			opposite = !opposite;
		if(opposite) {
			DriveSpeed1 = DriveSpeed1 * -1;
			DriveSpeed2 = DriveSpeed2 * -1;
		}
		if(proxy->get("joy1b4")) {
			DriveSpeed1=DriveSpeed2=1.00;
		} else if(proxy->get("joy1b5")){
			DriveSpeed1=DriveSpeed2=-1.00;
		} else {
			DriveSpeed1=DriveSpeed2=0;
		}
		FrontLeft.Set(DriveSpeed1);
		RearLeft.Set(DriveSpeed1);
		FrontRight.Set(-DriveSpeed2);
		RearRight.Set(-DriveSpeed2);
		
       printf("Speed1: %2.2f Speed2: %2.2f\r", DriveSpeed1, DriveSpeed2);
			// Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
};
