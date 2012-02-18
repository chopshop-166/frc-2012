/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: BridgeBalance.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Code for balancing on the bridge
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "Template" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskTemplate" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "BridgeBalance.h"

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
class BridgeBalanceLog : public MemoryLog
{
public:
	BridgeBalanceLog() : MemoryLog(
			sizeof(struct abuf), BridgeBalance_CYCLE_TIME, "BridgeBalance",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~BridgeBalanceLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int BridgeBalanceLog::PutOne(void)
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
unsigned int BridgeBalanceLog::DumpBuffer(char *nptr, FILE *ofile)
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
BridgeBalance166::BridgeBalance166(void):
	Balance(INCLINOMETER_CHANNEL_A, INCLINOMETER_CHANNEL_B)
	{
	Start((char *)"166BridgeBalanceTask", BridgeBalance_CYCLE_TIME);
	TempAngle = 0;
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
BridgeBalance166::~BridgeBalance166(void)
{
	return;
};
	
// Main function of the task
int BridgeBalance166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	BridgeBalanceLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 BridgeBalance task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	proxy->add("RobotAngle");
	Balance.Start();
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// <<CHANGEME>>
		// Insert your own logic here
		TempAngle = Balance.Get();
		proxy->set("RobotAngle",TempAngle);
#if 0
		if(proxy->get("JOY1B5")) {
			proxy->UseUserJoystick(1,0);
			proxy->UseUserJoystick(2,0);
			if(TempAngle>10) {
				//robotgoforward
				proxy->set(DRIVE_1_JOYSTICK_Y, 75);
				proxy->set(DRIVE_2_JOYSTICK_Y, 75);
			}
			else if(TempAngle<-10) {
				//robotgobackward
				proxy->set(DRIVE_1_JOYSTICK_Y, -75);
				proxy->set(DRIVE_2_JOYSTICK_Y, -75);
			} else {
				proxy->set(DRIVE_1_JOYSTICK_Y, 0);
				proxy->set(DRIVE_2_JOYSTICK_Y, 0);	
			}
		} else {
			proxy->UseUserJoystick(1,1);
			proxy->UseUserJoystick(2,1);
		}
#endif
		printf("Angle: %d\r", TempAngle);
        // Logging any values
		// <<TODO>>
		// Make this match the declaraction above
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
