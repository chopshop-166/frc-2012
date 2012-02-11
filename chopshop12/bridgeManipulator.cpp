/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: bridgeManipulator.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: bridgeManipulator source file for tasks, with bridgeManipulator functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "bridgeManipulator" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "bridgeManipulator" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "bridgeManipulator.h"

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
class bridgeManipulatorLog : public MemoryLog
{
public:
	bridgeManipulatorLog() : MemoryLog(
			sizeof(struct abuf), bridgeManipulator_CYCLE_TIME, "bridgeManipulator",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~bridgeManipulatorLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int bridgeManipulatorLog::PutOne(void)
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
unsigned int bridgeManipulatorLog::DumpBuffer(char *nptr, FILE *ofile)
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
bridgeManipulator166::bridgeManipulator166(void):
	bridgeManipulator(BRIDGE_MANIPULATOR),
	bottomLimit(BRIDGE_BOTTOM_LIMIT),
	topLimit(BRIDGE_TOP_LIMIT)
{
	Start((char *)"166bridgeManipulatorTask", bridgeManipulator_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
bridgeManipulator166::~bridgeManipulator166(void)
{
	return;
};
	
// Main function of the task
int bridgeManipulator166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	bridgeManipulatorLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 bridgeManipulator task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	proxy->TrackNewpress(BM_BUTTON);
	ManipulatorSpeed = .3;
	ManipulatorState=1;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		if(proxy->get(BM_BUTTON_N)){
			ManipulatorState = ManipulatorState * -1;
		}
		if((ManipulatorState>0) && topLimit.Get()){
			ManipulatorSpeed = 0;
		} else if((ManipulatorState<0) && bottomLimit.Get()) {
			ManipulatorSpeed = 0;
		} else { 
			ManipulatorSpeed = 0.3 * ManipulatorState;
		}
		
		bridgeManipulator.Set(ManipulatorSpeed);
		printf("Button: %1.f Speed: %2.2f State: %d\r", proxy->get(BM_BUTTON_N),ManipulatorSpeed, ManipulatorState);
		//commented out code
		/*
		switch (state){
			case(BM_BUTTON_RELEASED):
				if(!topLimit.Get())
				    bridgeManipulator.Set(.5);
				if(topLimit.Get())
					bridgeManipulator.Set(0);
				else if (proxy-> get(BM_BUTTON))
					state = BM_BUTTON_PUSHED;
				break;
			
			case(BM_BUTTON_PUSHED):
				if(!bottomLimit.Get())
					bridgeManipulator.Set(-.5);
				if(bottomLimit.Get())
					bridgeManipulator.Set(0);
				else if (!proxy -> get(BM_BUTTON))
					state = BM_BUTTON_RELEASED;
				break;	
		}
		*/
		//printf("bottomLimit:%d,topLimit:%d\n",bottomLimit.Get(),topLimit.Get());
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
