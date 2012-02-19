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
	float top_jaguar_speed;           // Speed of top Jaguars
	float bottom_jaguar_speed;        // Speed of bottom Jaguars
};

//  Memory Log
// <<CHANGEME>>
class ShooterLog : public MemoryLog
{
public:
	ShooterLog() : MemoryLog(
			sizeof(struct abuf), TEMPLATE_CYCLE_TIME, "Shooter",
			"Seconds,Nanoseconds,Elapsed Time\n" // Put the names of the values in here, comma-seperated
			) {
		return;
	};
	~ShooterLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(float s1, float s2);     // Log the values needed-add in arguments
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ShooterLog::PutOne(float speed1, float speed2)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->top_jaguar_speed = speed1;
		ob->bottom_jaguar_speed = speed2;
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
	fprintf(ofile, "%u,%u, %4.5f, %4.5f, %4.5f\n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->top_jaguar_speed,
			ab->bottom_jaguar_speed
	);
	// Done
	return (sizeof(struct abuf));
}


// task constructor
Shooter::Shooter(void):
	ShooterJagTopA(SHOOTER_JAG_TOP_A),
	ShooterJagTopB(SHOOTER_JAG_TOP_B),
	ShooterJagBottomA(SHOOTER_JAG_BOTTOM_A),
	ShooterJagBottomB(SHOOTER_JAG_BOTTOM_B)
{
	Start((char *)"166Shooter", TEMPLATE_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	P=0.9;
	I=0.01;
	D=0;
	const double MaxOutputVolts=6.00;
	//Top Jag A

	ShooterJagTopA.ConfigEncoderCodesPerRev(360);
#if PID
	ShooterJagTopA.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
	ShooterJagTopA.ChangeControlMode(CANJaguar::kSpeed);
	ShooterJagTopA.SetPID(P,I,D);
	//ShooterJagTopA.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagTopA.EnableControl(0);
#endif
	//Top Jag B
	ShooterJagTopB.ChangeControlMode(CANJaguar::kVoltage);
	//ShooterJagTopB.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagTopB.EnableControl();
	//Bottom Jag A
#if PID
	ShooterJagBottomA.ConfigEncoderCodesPerRev(360);
	ShooterJagBottomA.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
	ShooterJagBottomA.ChangeControlMode(CANJaguar::kSpeed);
	ShooterJagBottomA.SetPID(P,I,D);
	//ShooterJagBottomA.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagBottomA.EnableControl(0);
#endif
	//Bottom Jag B
	ShooterJagBottomB.ChangeControlMode(CANJaguar::kVoltage);
	//ShooterJagBottomB.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagBottomB.EnableControl();
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
	DPRINTF(LOG_DEBUG,"In the 166 Shooter task\n");
	
	// Wait for Robot go-ahead (e.g. entering Autonomous or Tele-operated mode)
	// lHandle = Robot::getInstance() MUST go after this, otherwise code breaks
	WaitForGoAhead();
	
	// Register our logger
	lHandle = Robot::getInstance();
	lHandle->RegisterLogger(&sl);
	proxy->TrackNewpress("joy1b1");
	//Proportianal Change
	proxy->TrackNewpress("joy1b6");
	proxy->TrackNewpress("joy1b7");
	//Integral Change
	proxy->TrackNewpress("joy1b3");
	proxy->TrackNewpress("joy1b2");
	//Derivative Change
	proxy->TrackNewpress("joy1b11");
	proxy->TrackNewpress("joy1b10");
	//Change Incremental Value
	proxy->TrackNewpress("joy1b8");
	proxy->TrackNewpress("joy1b9");
	//Change Speed
	proxy->TrackNewpress("joy1b4");
	proxy->TrackNewpress("joy1b5");
	proxy->TrackNewpress("joy2b4");
	proxy->TrackNewpress("joy2b5");
	float Speed=0, Speed2=0;
	float changevalue=0;
	float MasterSpeedTop=0, MasterSpeedBottom=0;
	float joystickspeed=0;
	// General main loop (while in Autonomous or Tele mode)
	while (true) {
		
		//Slave 1 jaguar per axle to the other jaguar
		MasterSpeedTop = ShooterJagTopA.GetOutputVoltage();
		MasterSpeedBottom = ShooterJagBottomA.GetOutputVoltage();
		ShooterJagTopB.Set(MasterSpeedTop);
		ShooterJagBottomB.Set(MasterSpeedBottom);
#if 0
		//set speed
		if(proxy->get("joy1b8", true)) {
			changevalue+=0.01;
		} else if(proxy->get("joy1b9", true)){
			changevalue-=0.01;
		}
		//Set Proportional
		if(proxy->get("joy1b6n", true)) {
			P-=changevalue;
		} else if(proxy->get("joy1b7n", true)){
			P+=changevalue;
		}
		//Set Integral
		if(proxy->get("joy1b3n", true)) {
			I-=changevalue;
		} else if(proxy->get("joy1b2n",true)){
			I+=changevalue;
		}
		//Set Derivative
		if(proxy->get("joy1b11n", true)) {
			D-=changevalue;
		} else if(proxy->get("joy1b10n", true)){
			D+=changevalue;
		}
		if(proxy->get("joy1b1n", true)){
			//Set PID values for Top Jag A
			ShooterJagTopA.SetPID(P,I,D);
			ShooterJagTopA.EnableControl(0);
			//Set PID values for Bottom Jag A
			ShooterJagBottomA.SetPID(P,I,D);
			ShooterJagBottomA.EnableControl(0);
		}
#endif
		//Set Speed
		if(proxy->get("joy1b4n", true)) {
			Speed+=50;
		} else if(proxy->get("joy1b5n", true)) {
			Speed-=50;
		}
		if(proxy->get("joy2b4n", true)) {
			Speed2+=50;
		} else if(proxy->get("joy2b5n", true)) {
			Speed2-=50;
		}
		//printf("Speed: %f\r", Speed);
		//Press trigger to make motors go
		if(proxy->get("joy2b1")||proxy->get("joy1b1")) {
			ShooterJagTopA.Set(-(Speed));
			ShooterJagBottomA.Set(Speed2);
		} else {
			ShooterJagTopA.Set(0);
			ShooterJagBottomA.Set(0);
		}
#if 0
		printf("Change: %f P: %f I: %f D: %f Speed: %d Speed: %f\r",
				changevalue, P, I, D,
				Speed, ShooterJagTopA.GetSpeed());
#endif
		printf("RPM Top %f ACT: %f Volts %f RPM Bottom %f ACT %f Volts %f\r", Speed, ShooterJagTopA.GetSpeed(),ShooterJagTopA.GetOutputVoltage(), Speed2, ShooterJagBottomA.GetSpeed(), ShooterJagBottomA.GetOutputVoltage());
	//a switch that takes a ballcount from the proxy, if its 0, 
	//the motors spin slowly, otherwise, code runs normally.
	/*
	switch (proxy->get(SHOOTER_TRIGGER) ) 
	{
		case true:
			speed1 = proxy->get("vo"); //get speed from proxy in feet per second	
			speed1 = speed1 / 60 * 2.083333333333333; // convert fps to rpm
			
			speed2 = speed1 * -1; // set bottom speed to opposite of top
			speed2 = speed1 * BackspinFactor; //allows the top and bottom to go at different speeds for goodness
			break;
		case false:
			speed1 = 1000;
			speed2 = 1000;
			break;
	}
	ShooterJagTopA.Set(speed1);
	ShooterJagTopB.Set(speed2);
	*/
		// Make this match the declaraction above
		sl.PutOne(ShooterJagTopA.GetSpeed(), ShooterJagBottomA.GetSpeed());
		
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};
