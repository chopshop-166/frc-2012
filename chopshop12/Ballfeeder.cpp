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
			sizeof(struct abuf), BALLFEEDER_CYCLE_TIME, "BallFeeder",
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
	BallCount = 0;
	feedspeed = 0;
	waitTimer = 0;
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
	proxy->add("BallCount");
		
	int Sens0Ave[NUMTOAVERAGE]={0};
	int Sens1Ave[NUMTOAVERAGE]={0};
	int Sens2Ave[NUMTOAVERAGE]={0};
	int Sens3Ave[NUMTOAVERAGE]={0};
	int counter = 0;
	
	int Average0 = 0;
	int Average1 = 0;
	int Average2 = 0;
	int Average3 = 0;
	
	int PrevBallCount = 0;
	
    // General main loop (while in Autonomous or Tele mode)
	while (true) {
		 //BallCount = number of balls the robot has
		//These are normally High so we need to invert them to detect a ball
		Sens0Ave[counter%NUMTOAVERAGE]=!(BallLocation0.Get());
		Sens1Ave[counter%NUMTOAVERAGE]=!(BallLocation1.Get());
		Sens2Ave[counter%NUMTOAVERAGE]=!(BallLocation2.Get());
		Sens3Ave[counter%NUMTOAVERAGE]=!(BallLocation3.Get());
		counter++;
		
		for( int i=0;i<NUMTOAVERAGE;i++){
			Average0+=Sens0Ave[i];
			Average1+=Sens1Ave[i];
			Average2+=Sens2Ave[i];
			Average3+=Sens3Ave[i];
		}
		
		Average0=(Average0/NUMTOAVERAGE);
		Average1=(Average1/NUMTOAVERAGE);
		Average2=(Average2/NUMTOAVERAGE);
		Average3=(Average3/NUMTOAVERAGE);

		BallCount = (Average3 + Average2 + Average1);
		proxy->TrackNewpress("joy3b1");
		proxy->set("BallCount",BallCount);
		//printf("\rBall Count: %d \t", BallCount);
		//printf("Ball 0: %d ball 1: %d Ball 2: %d Ball 3: %d\r",
				//BallLocation0.Get(), BallLocation1.Get(), BallLocation2.Get(), BallLocation3.Get());
		//to shoot, pull trigger
#if 0
		if (proxy->get("joy3b1"))
		{
			PrevBallCount = BallCount;
			if(BallCount != (PrevBallCount-1)) {
				feedspeed = BALLFEED;
			} else {
				feedspeed = 0;
			}
		}
		//ball incoming

		else {
			switch(FeedState) {
				case Stopped:
					printf("I am Stopped");
					feedspeed=0;
					if(!BallLocation0.Get()) {
						waitTimer=0;
						FeedState = Waiting;
					}
					break;
				case Waiting:
					feedspeed = 0;
						waitTimer++;
						if(waitTimer==70)
							FeedState=CollectionStarted;
					break;
				case CollectionStarted:
					waitTimer = 0;
					feedspeed = -.20;
					switch(BallCount){
						case 0:
							FeedState = Store1Ball;
							break;
						case 1:
							FeedState = Store2Ball;
							break;
						case 2:
							FeedState = Store3Ball;
							break;
					}
					break;
				case Store1Ball:
					printf("I am Storing 1 Ball");
					if(!BallLocation1.Get()){
						FeedState = Stopped;
					}
					break;
				case Store2Ball:
					printf("I am Storing 2 Ball");
					if(!BallLocation2.Get()){
						FeedState = Stopping;
					}
					waitTimer++;
					break;
				case Store3Ball:
					printf("I am Storing 3 Ball");
					if(!BallLocation3.Get()){
						FeedState = Stopped;
					}
					break;
				case Stopping:
					printf("I Am STOPPING!!!");
					if(waitTimer>=10){
						FeedState = Stopped;
					}
					waitTimer++;
					break;
				default:
					printf("YO DUMB!\n");
					break;
			}
		}
		printf("Ball Count: %d Timer: %d\r", BallCount, waitTimer);
#endif
		if(proxy->get("joy2b3")){
			feedspeed = BALLFEED;
		} else {
			feedspeed = 0;
		}
		BallFeed.Set(feedspeed);
		sl.PutOne();		
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
};
