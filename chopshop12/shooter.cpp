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
	float TheoreticalSpeedTop;        // Speed that top jags should be
	float TheoreticalSpeedBottom;     // Speed that bottom jags should be
	float ActualSpeedTop;			      // Actual current speed of top jags
	float ActualSpeedBottom;			  // Actual current speed of bottom jags
//Voltages for all the motors
	float VoltageTopA;
	float VoltageTopB;
	float VoltageBottomA;
	float VoltageBottomB;
		
};

//  Memory Log
// <<CHANGEME>>
class ShooterLog : public MemoryLog
{
public:
	ShooterLog() : MemoryLog(
			sizeof(struct abuf), SHOOTER_CYCLE_TIME, "Shooter",
			 // Put the names of the values in here, comma-seperated	
			"Seconds,Nanoseconds,Elapsed Time,\
			TheoreticalSpeedTop,TheoreticalSpeedBottom,\
			ActualSpeedTop,ActualSpeedBottom,\
			VoltageTopA,VoltageTopB,VoltageBottomA,VoltageBottomB\n"
			){
	return;
	};
	
	~ShooterLog() {return;};
	unsigned int DumpBuffer(          // Dump the next buffer into the file
			char *nptr,               // Buffer that needs to be formatted
			FILE *outputFile);        // and then stored in this file
	// <<CHANGEME>>
	unsigned int PutOne(
		// Log the values needed-add in arguments
		float TheoreticalSpeedTop,        
		float TheoreticalSpeedBottom,    
		float ActualSpeedTop,			      
		float ActualSpeedBottom,			  
		float VoltageTopA,
		float VoltageTopB,
		float VoltageBottomA,
		float VoltageBottomB);
};

// Write one buffer into memory
// <<CHANGEME>>
unsigned int ShooterLog::PutOne(
		float TheoreticalSpeedTop, 
		float TheoreticalSpeedBottom,
		float ActualSpeedTop,
		float ActualSpeedBottom,
		float VoltageTopA,
		float VoltageTopB,
		float VoltageBottomA,
		float VoltageBottomB)
{
	struct abuf *ob;               // Output buffer
	
	// Get output buffer
	if ((ob = (struct abuf *)GetNextBuffer(sizeof(struct abuf)))) {
		
		// Fill it in.
		clock_gettime(CLOCK_REALTIME, &ob->tp);
		ob->TheoreticalSpeedTop = TheoreticalSpeedTop;
		ob->TheoreticalSpeedBottom = TheoreticalSpeedBottom;
		ob->ActualSpeedTop = ActualSpeedTop;
		ob->ActualSpeedBottom = ActualSpeedBottom;
		ob->VoltageTopA = VoltageTopA;
		ob->VoltageTopB = VoltageTopB;
		ob->VoltageBottomA = VoltageBottomA;
		ob->VoltageBottomB = VoltageBottomB;
		
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
	fprintf(ofile, "%u,%u, %4.5f, %4.5f, %4.5f, "
			"%fTheoreticalSpeedTop,"
			"%fTheoreticalSpeedBottom,"
			"%fActualSpeedtop,"
			"%fActualSpeedBottom,"
			"%fVoltageTopA,"
			"%fVoltageTopB,"
			"%fVoltageBottomA,"
			"%fVoltageBottomB, \n",
			ab->tp.tv_sec, ab->tp.tv_nsec,
			((ab->tp.tv_sec - starttime.tv_sec) + ((ab->tp.tv_nsec-starttime.tv_nsec)/1000000000.)),
			ab->TheoreticalSpeedTop,
			ab->TheoreticalSpeedBottom,
			ab->ActualSpeedTop,
			ab->ActualSpeedBottom,
			ab->VoltageTopA,
			ab->VoltageTopB,
			ab->VoltageBottomA,
			ab->VoltageBottomB
			
	);
	// Done
	return (sizeof(struct abuf));
}


// task constructor
Shooter::Shooter(void):
	ShooterJagTopA(SHOOTER_JAG_TOP_A),
	ShooterJagTopB(SHOOTER_JAG_TOP_B),
	ShooterJagBottomA(SHOOTER_JAG_BOTTOM_B),
	ShooterJagBottomB(SHOOTER_JAG_BOTTOM_A)
{
	Start((char *)"166Shooter", SHOOTER_CYCLE_TIME);
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
	
	proxy->TrackNewpress("joy3b7");
	proxy->TrackNewpress("joy3b8");
	proxy->TrackNewpress("joy3b10");
	proxy->TrackNewpress("joy3b11");
	float TopSpeed=0, BottomSpeed=0;
	float changevalue=0;
	float MasterSpeedTop=0, MasterSpeedBottom=0;
	int loopcounter=0;
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
		
		 if(proxy->get(SHOOTER_TOP_SPEED_INCREASE, true)) {
			TopSpeed+=50;
		} else if(proxy->get(SHOOTER_TOP_SPEED_DECREASE, true)) {
			TopSpeed-=50;
		}
		if(proxy->get(SHOOTER_BOTTOM_SPEED_INCREASE, true)) {
			BottomSpeed+=50;
		} else if(proxy->get(SHOOTER_BOTTOM_SPEED_DECREASE, true)) {
			BottomSpeed-=50;
		}
		
		//printf("Speed: %f\r", Speed);
		//Press trigger to make motors go
		if(proxy->get(SHOOTER_TRIGGER)) {
			ShooterJagTopA.Set(-(KEY_SPEED_TOP));
			ShooterJagBottomA.Set(KEY_SPEED_BOTTOM);
		} else if (proxy->get(SHOOTER_MANUAL_TRIGGER)) {
			ShooterJagTopA.Set(TopSpeed);
			ShooterJagBottomA.Set(BottomSpeed);
		} else {
			ShooterJagTopA.Set(0);
			ShooterJagBottomA.Set(0);
		}
		
#if 0
		printf("Change: %f P: %f I: %f D: %f Speed: %d Speed: %f\r",
				changevalue, P, I, D,
				TopSpeed, ShooterJagTopA.GetSpeed());
#endif
		if ((loopcounter++ %5)==0)
		{
		//printf("RPM Top %f ACT: %f Volts %f RPM Bottom %f ACT %f Volts %f\r", TopSpeed, ShooterJagTopA.GetSpeed(),ShooterJagTopA.GetOutputVoltage(), BottomSpeed, ShooterJagBottomA.GetSpeed(), ShooterJagBottomA.GetOutputVoltage());
		//printf("RPM Top: %f RPM Bottom: %f\r", ShooterJagTopA.GetSpeed(), ShooterJagBottomA.GetSpeed());
		}
	//a switch that takes a ballcount from the proxy, if its 0, 
	//the motors spin slowly, otherwise, code runs normally.
	/*
	switch (proxy->get(SHOOTER_TRIGGER) ) 
	{
		case true:
			speed1 = proxy->get("initial_velocity"); //get speed from proxy in feet per second	
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
	ShooterJagBottomA.Set(speed2);
	*/
		// Make this match the declaraction above
		sl.PutOne(
				speed1,
				speed2,
				ShooterJagTopA.Get(),
				ShooterJagBottomA.Get(),
				0.1,//VoltageTopA,
				0.1,//VoltageTopB,
				0.1,//VoltageBottomA,
				0.1//VoltageBottomB
				);
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};

