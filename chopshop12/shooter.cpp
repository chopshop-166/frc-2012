/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskTemplate.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template source file for tasks, with template functions
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
#include "shooter.h"
#include "CANJaguar.h"
#include "PIDController.h"

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
class ShooterLog : public MemoryLog
{
public:
	ShooterLog() : MemoryLog(
			sizeof(struct abuf), TEMPLATE_CYCLE_TIME, "template",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~ShooterLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(void);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ShooterLog::PutOne(void)
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
unsigned int ShooterLog::DumpBuffer(char *nptr, FILE *ofile)
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
Shooter::Shooter(void):
	ShooterJagTopA(SHOOTERJAGTOPA),
	ShooterJagTopB(SHOOTERJAGTOPB),
	ShooterJagBottomA(SHOOTERJAGBOTTOMA),
	ShooterJagBottomB(SHOOTERJAGBOTTOMB)
{
	Start((char *)"166Templateask", TEMPLATE_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	//Front Left
		ShooterJagTopA.ConfigEncoderCodesPerRev(360);
		ShooterJagTopA.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		ShooterJagTopA.SetPID(0.150, 0.004, 0.00);
		ShooterJagTopA.EnableControl(0);
		//Front Right
		ShooterJagTopB.ConfigEncoderCodesPerRev(360);
		ShooterJagTopB.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		ShooterJagTopB.SetPID(0.150, 0.004, 0.00);
		ShooterJagTopB.EnableControl(0);
		//Back Left
		ShooterJagBottomA.ConfigEncoderCodesPerRev(360);
		ShooterJagBottomA.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		ShooterJagBottomA.SetPID(0.150, 0.004, 0.00);
		ShooterJagBottomA.EnableControl(0);
		//Back Right
		ShooterJagBottomB.ConfigEncoderCodesPerRev(360);
		ShooterJagBottomB.SetSpeedReference(CANJaguar::kSpeedRef_Encoder);
		ShooterJagBottomB.SetPID(0.150, 0.005, 0.00);
		ShooterJagBottomB.EnableControl(0);
	// Register the proxy
	proxy = Proxy::getInstance();
	return;
};
	
// task destructor
Shooter::~Shooter(void)
{
	return;
};
	
// Main function of the task
int Shooter::Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10)
{
	ShooterLog sl;                   // log
	
	// Let the world know we're in
	DPRINTF(LOG_DEBUG,"In the 166 Template task\n");
	
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
			//a switch that takes a ballcount from the proxy, if its 0, 
			//the motors spin slowly, otherwise, code runs normally.
	switch (proxy->get ("BallCount") > 0) 
	{
		case true:
			speed1 = proxy->get("vo"); //get speed from proxy in feet per second	
			speed1 = speed1 / 60 * 2.083333333333333; // convert fps to rpm
			
			speed2 = speed1 * -1; // set bottom speed to opposite of top
			speed2 = speed1 * BaskspinFactor; //allows the top and bottom to go at different speeds for goodness
						
			ShooterJagTopA.Set(speed1);
			ShooterJagTopB.Set(speed1);
			ShooterJagBottomA.Set(speed2); //set ALL the speeds!
			ShooterJagBottomB.Set(speed2);
			break;
		case false:
			ShooterJagTopA.Set(1000);
			ShooterJagTopB.Set(1000);
			ShooterJagBottomA.Set(10000); //make slow the speeds of the motors 
			ShooterJagBottomB.Set(1000);
			break;
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
