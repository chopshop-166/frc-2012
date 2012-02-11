/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskBallFeeder.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: BallFeeder source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
	
/*------------------------------------------------------------------------------*/
/* Find & Replace "BallFeeder" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TaskBallFeeder" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/
	
#include "WPILib.h"
#include "BallFeeder.h"
	
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
class BallFeederLog : public MemoryLog
{	
public:
	BallFeederLog() : MemoryLog(
			sizeof(struct abuf), BALLFEEDER_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~BallFeederLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};	
	
// Write one buffer into memory
// <<CHANGEME>>
unsigned int BallFeederLog::PutOne(void)
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
unsigned int BallFeederLog::DumpBuffer(char *nptr, FILE *ofile)
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
BallFeeder166::BallFeeder166(void):
	BallFeed(BALL_FEED),//victor controlling feeder
	//digital inputs for ball locations
	BallLocation0(BALL_LOCATION_0),
	BallLocation1(BALL_LOCATION_1),
	BallLocation2(BALL_LOCATION_2),
	BallLocation3(BALL_LOCATION_3)
{	
	Start((char *)"166BallFeederTask", BALLFEEDER_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};	
	
// task destructor
BallFeeder166::~BallFeeder166(void)
{		
	return;
};	
	
// Main function of the task
int BallFeeder166::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{	
	BallFeederLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 BallFeeder task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go pafter this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	//send ball location digital inputs to the proxy
	proxy->add("BallLocation0");
	proxy->add("BallLocation1");
	proxy->add("BallLocation2");
	proxy->add("BallLocation3");
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		proxy->set("BallLocation0",BallLocation0.Get());
		proxy->set("BallLocation1",BallLocation1.Get());
		proxy->set("BallLocation2",BallLocation2.Get());
		proxy->set("BallLocation3",BallLocation3.Get());
		 //BallCount = number of balls the robot has
		
		int Sens0Ave[50];
		int Sens1Ave[50];
		int Sens2Ave[50];
		int Sens3Ave[50];
		int counter;
		
		Sens0Ave[counter%50]=BallLocation0.Get();
		Sens1Ave[counter%50]=BallLocation1.Get();
		Sens2Ave[counter%50]=BallLocation2.Get();
		Sens3Ave[counter%50]=BallLocation3.Get();
		counter ++;
		
		int Average0;
		int Average1;
		int Average2;
		int Average3;
		
		for( int i=0;i<50;i++){
			Average0+=Sens0Ave[i];
			Average1+=Sens1Ave[i];
			Average2+=Sens2Ave[i];
			Average3+=Sens3Ave[i];
		}
		proxy->set("BallCount",Average3 + Average2 + Average1 + Average0);
		Average0=Average0/50;
		Average1=Average1/50;
		Average2=Average2/50;
		Average3=Average3/50;
		
		//to shoot, pull trigger
		if (proxy->get("Joy1b1"))
		{
			//feeder has 3 balls
			if (BallCount == 3){
				while(BallCount>2)
					feedspeed = BALLFEED;
				feedspeed = 0;
			}
			//feeder has 2 balls
			else if (BallCount == 2){
				while(BallCount>1)
					feedspeed = BALLFEED;
				feedspeed = 0;
			}
			//feeder has 1 ball
			else if (BallCount == 1){
				while(BallCount>0)
					feedspeed = BALLFEED;
				feedspeed = 0;
			}
			else
				feedspeed=BALLFEED;
		}
		//ball incoming
		else if ((BallLocation0.Get()== true)&&(BallLocation1.Get()== false))
		{
			feedspeed=BALLFEED;
		}
		//ball incoming while ball 1 is in
		else if ((BallLocation0.Get()== true)&&(BallLocation1.Get()== true)&&(BallLocation2.Get()== false))
		{
			feedspeed=BALLFEED;
		}
		//ball incoming with ball 1 & 2 in
		else if ((BallLocation0.Get()== true)&&(BallLocation1.Get()== true)&&(BallLocation2.Get()== true)&&(BallLocation3.Get()== false))
		{
			feedspeed=BALLFEED;
		}		
		else
		{
			feedspeed=0;
		}
		
			sl.PutOne();		
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	

	};
