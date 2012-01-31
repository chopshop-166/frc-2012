/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TargetingInfo.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: TargetingInfo source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "TargetingInfo" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TargetingInfo" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "TargetingInfo.h"

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
class TargetingInfoLog : public MemoryLog
{
public:
	TargetingInfoLog() : MemoryLog(
			sizeof(struct abuf), TARGETINGINFO_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~TargetingInfoLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int TargetingInfoLog::PutOne(void)
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
unsigned int TargetingInfoLog::DumpBuffer(char *nptr, FILE *ofile)
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
TargetingInfo166::TargetingInfo166(void)
{
	Start((char *)"166TargetingInfoTask", TARGETINGINFO_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
TargetingInfo166::~TargetingInfo166(void)
{
	return;
};
	
// Main function of the task
int TargetingInfo166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	TargetingInfoLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 TargetingInfo task\n");
	
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
		
		
		//calling M's function to get necessary data
		//ReturnReport(); ADD IN LATER!!!!!!!!!!!!!!!!!!!!!!!
		
		
		
		
		//This section claculates the distance from the target (parallel to the ground)
		distance=DISTANCECALIBRATION*imageheight/targetheight;
		calctargetwidth=targetheight*24/18; //24/18 is ratio of width to height of vision target
		realtargetwidth=54;//TEMPORARY!!!!!!!
		angle=(PI/2)-(acos(realtargetwidth/calctargetwidth));
		deltax=distance/(sin(angle));

		
		
		
		//This section calculates the launch velocity needed (vo)
		vo=sqrt((GRAVITY*pow(deltax, 2))/(2*(pow(cos(LANGLE),2))*(deltax*tan(LANGLE)-TOPDELTAY))); //there is a notepad somewhere with calculations
		
		
		
		//this section figures out whether or not the ball can enter at the calulated trajectory
		vox=vo*cos(LANGLE);
		time=deltax/vox;
		voy=vo*sin(LANGLE);
		vfy=voy-GRAVITY*time;
		eangle=atan(vfy/vox);
		if (eangle>27*180/PI)
		{
		    cout<<"Safe to launch";
		}
		else
		{
		    cout<<"Not safe to launch";
		}
        // Logging any values
		// <<CHANGEME>>
		// Make this match the declaraction above
		sl.PutOne();
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
