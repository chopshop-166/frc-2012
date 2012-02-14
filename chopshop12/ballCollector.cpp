/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: ballCollector.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: ballCollector source file for tasks, with ballCollector functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "ballCollector" with the name you would like to give this task     */
/* Find & Replace "ballCollector" with the name you would like to give this task      */
/* Find & Replace "ballCollector" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "ballCollector.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

// Sample in memory buffer
struct abuf
{
	struct timespec tp;               // Time of snapshot
	int state;
	// Any values that need to be logged go here
	// <<CHANGEME>>
};

//  Memory Log
// <<CHANGEME>>
class ballCollectorLog : public MemoryLog
{
public:
	ballCollectorLog() : MemoryLog(
			sizeof(struct abuf), ballCollector_CYCLE_TIME, "ballCollector",
			"Seconds,Nanoseconds,Elapsed Time,state\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~ballCollectorLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(int state);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ballCollectorLog::PutOne(int state)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob -> state = state;
		return (sizeof(struct abuf));
	}
	
	// Did not get a buffer. Return a zero length
	return (0);
}

// Format the next buffer for file output
unsigned int ballCollectorLog::DumpBuffer(char *nptr, FILE *ofile)
{
	struct abuf *ab = (struct abuf *)nptr;
	
	// Output the data into the file
	fprintf(ofile, "%u,%u,%4.5f,%d\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab -> state
			
	);
	
	// Done
	return (sizeof(struct abuf));
}


// task constructor
ballCollector166::ballCollector166(void): ballCollector (BALL_COLLECTOR)
{
	printf("in constructor\n");
	Start((char *)"166ballCollectorTask", ballCollector_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
ballCollector166::~ballCollector166(void)
{
	return;
};
	
// Main function of the task
int ballCollector166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	ballCollectorLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 ballCollector task\n");
	printf("in main task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	int pcounter = 0;
	state = BC_ROLL_INWARD;
	proxy->TrackNewpress("joy1b3");
    // General main loop (while in Autonomous or Tele mode)
	while (true) {  //infinite while loop
		if (proxy->get("matchTimer")>=117) { //once purge mode is active the state changes to outward
			state = BC_ROLL_OUTWARD;
		}
		
		switch(state){ // switches between inward and outward
			case(BC_ROLL_INWARD): 
				if(proxy->get("BallCount") > 3) // if the ballcount is less then three it sets the motor to go forward at halfspeed
					state = BC_ROLL_OUTWARD;
				else
					ballCollector.Set(0.25);
				break;
			case(BC_ROLL_OUTWARD):
				if(proxy->get("BallCount") < 3) // if the ballcount is greater than or equal to 3, it sets the motor to drive backwards at half speed
					state = BC_ROLL_INWARD;
				else
					ballCollector.Set(-0.25);	
				break;
		}
		sl.PutOne((int)state);
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return(0);
	
}
