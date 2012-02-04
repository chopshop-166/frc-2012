/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskTurret.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Turret source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
	
/*------------------------------------------------------------------------------*/
/* Find & Replace "Turret" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskTurret" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/
	
#include "WPILib.h"
#include "Turret.h"
	
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
class TurretLog : public MemoryLog
{	
public:
	TurretLog() : MemoryLog(
			sizeof(struct abuf), TURRET_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~TurretLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};	
	
// Write one buffer into memory
// <<CHANGEME>>
unsigned int TurretLog::PutOne(void)
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
unsigned int TurretLog::DumpBuffer(char *nptr, FILE *ofile)
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
Turret166::Turret166(void):turretpot(TURRET_POT),rotateturret(TURRET_JAG),leftlimit(TURRET_LEFT_LIMIT),rightlimit(TURRET_RIGHT_LIMIT)
{	
	Start((char *)"166TurretTask", TURRET_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};	
	
// task destructor
Turret166::~Turret166(void)
{	
	return;
};	
int Turret166::volttoangle(float volts) //class that will convert volt to angle
{	
	return (int) volts;
}	
	
// Main function of the task
int Turret166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{	
	TurretLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Turret task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	
	float rspeed;
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// <<CHANGEME>>
		// Insert your own logic here
		
		rspeed=(proxy->get("Joy1y")*proxy->get("Joy1y")); //joystick speed squared
		
		if(proxy->get("Joy1b1"))
			rspeed=(proxy->get("Joy1y")*proxy->get("Joy1y")*-1); //trigger reverses speed
		if(proxy->get("Joy1b2"))		//stop
			rspeed=0;
		if (leftlimit.Get())	
        	rspeed = 0;
        if (rightlimit.Get())
        	rspeed = 0;
        rotateturret.Set(rspeed)	;		//motor = joystick speed
		
        volt = turretpot.GetVoltage();		//voltage = what the pot picks up		
        printf("pot voltage: %f\r",volt);	//shows volts
        printf("speed: %f\r",rspeed);
        
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};	
	
	
