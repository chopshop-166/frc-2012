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
	float volt; 						// volt of ...
	bool leftlimit;
	bool rightlimit;
	
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
			"Seconds,Nanoseconds,Elapsed Time,Leftlimit,Rightlimit,Volt\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~TurretLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(float volt, bool leftlimit, bool rightlimit);     // Log the values needed-add in arguments
};	
	
// Write one buffer into memory
// <<CHANGEME>>
unsigned int TurretLog::PutOne(float volt,bool leftlimit,bool rightlimit)
{	
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->leftlimit = leftlimit;
		ob->rightlimit = rightlimit;
		ob->volt = volt;
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
	fprintf(ofile, "%u,%u,%4.5f,%d,%d,%2.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->leftlimit,
			ab->rightlimit,
			ab->volt
			// Add values here
			// <<CHANGEME>>
	);
	
	// Done
	return (sizeof(struct abuf));
}	
	
	
// task constructor
Turret166::Turret166(void):turretpot(TURRET_POT), rotateturret(TURRET_JAG)
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
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		// <<CHANGEME>>
		// Insert your own logic here
		
/* DETERMINING TURRET SPEEDS:
 * Emergency stop check*/
	if(proxy->get("Joy1b2"))		
		{
		rspeed=0;
		}

	else
		{
	/*
	 * Do we want to use the camera or the joystick?*/
		if(proxy->get("Joy1b1"))
		    {  
			rspeed=2* proxy->get("CameraX");
			/*if(rspeed<0)
				{rspeed=-.5;}
			else if(rspeed>0)
				{rspeed=.5;}
			else if (rspeed==0)
				{rspeed=0;}
			*/
			/*CameraX = proxy->get("CameraX");
			if (CameraX < -.5) 
				rspeed = 1;
			if (CameraX < 0) 
				rspeed = 0.5;
				{printf("Tuuret Moving Left \r");}
			if (CameraX = 0) 
				rspeed = 0;
				{printf("Turret CENTERED \r");}
			if (CameraX > 0)
				rspeed = -.5;
				{printf("Turret moving Right \r");}
			if (CameraX > .5)
				rspeed = 1;
			if (CameraX = 2)
				{
				if (centeroffset <THRESHOLD)
					rspeed = 1;
				if (centeroffset >THRESHOLD)
					rspeed = 0.5;
				if (centeroffset = 0)
					rspeed = 0;
				if (centeroffset <-THRESHOLD)
					rspeed = -1;
				if (centeroffset >-THRESHOLD)
					rspeed= -0.5;
				}*/
		    }
		else
		    {
			//Joystick Control Y-Axis
			if(proxy->get("Joy1y")>0)
				{
				rspeed=(proxy->get("Joy1y")*proxy->get("Joy1y"));
				}
			if(proxy->get("Joy1y")<0)
				{
				rspeed=(proxy->get("Joy1y")*proxy->get("Joy1y")*-1);
				}
		    }	
		}
      	
		//motor = joystick speed
		//sets rotateturret(CANJaguar) to rspeed	
/*APPLY SPEEDS*/
        //voltage = what the pot picks up
        volt = turretpot.GetVoltage();				
        centeroffset=volt-CENTERVOLTAGE;
        
        rotateturret.Set(rspeed);
        printf("pot voltage: %f speed: %f \n",volt,rspeed);	//shows volts
        
        /*if (TURRETANGLE<0)
        	rspeed = 0.1;
        else if (TURRETANGLE>0)
        	rspeed = -0.1;
        	*/
        rotateturret.Set(rspeed);
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne(rotateturret.GetForwardLimitOK(),rotateturret.GetReverseLimitOK(),volt);
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};	
