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
			"Seconds,Nanoseconds,Elapsed Time,"
			"TheoreticalSpeedTop,TheoreticalSpeedBottom,"
			"ActualSpeedTop,ActualSpeedBottom,"
			"VoltageTopA,VoltageTopB,VoltageBottomA,VoltageBottomB\n"
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
	fprintf(ofile, "%u,%u, %4.5f, %4.5f, %4.5f, %f, %f, %f, %f, %f, %f, %f, %f, \n",
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
	ShooterJagBottomA(SHOOTER_JAG_BOTTOM_A),
	ShooterJagBottomB(SHOOTER_JAG_BOTTOM_B)
{
	Start((char *)"166Shooter", SHOOTER_CYCLE_TIME);
	// ^^^ Rename those ^^^
	// <<CHANGEME>>
	P=0.9;
	I=0.01;
	D=0;
	TopSpeed=0;
	BottomSpeed=0;
	//Top Jag A
	ShooterJagTopA.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
	ShooterJagTopB.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
	ShooterJagBottomA.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
	ShooterJagBottomB.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
#if PID
	ShooterJagTopA.ConfigEncoderCodesPerRev(360);
	ShooterJagTopA.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
	ShooterJagTopA.ChangeControlMode(CANJaguar::kSpeed);
	ShooterJagTopA.SetPID(P,I,D);
	//ShooterJagTopA.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagTopA.EnableControl(0);
#else 
	//Top Jag B
	ShooterJagTopA.ChangeControlMode(CANJaguar::kVoltage);
	//ShooterJagTopB.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagTopA.EnableControl();
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
#else 
	//Bottom Jag B
	ShooterJagBottomA.ChangeControlMode(CANJaguar::kVoltage);
	//ShooterJagBottomB.ConfigMaxOutputVoltage(MaxOutputVolts);
	ShooterJagBottomA.EnableControl();
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
	
	proxy->TrackNewpress("joy3b6");
	proxy->TrackNewpress("joy3b7");
	proxy->TrackNewpress("joy3b10");
	proxy->TrackNewpress("joy3b11");
	//proxy->TrackNewpress(MAGIC_CONSTANT_INCREASE);
	//proxy->TrackNewpress(MAGIC_CONSTANT_DECREASE);
	float ManualTopSpeed=100, ManualBottomSpeed=100;
	float TopMasterVoltage=0, BottomMasterVoltage=0;
	float TopSpeedHigh=0, TopSpeedLow=0, TopSpeedCur=0;
	float BottomSpeedHigh=0, BottomSpeedLow=0, BottomSpeedCur=0;
	unsigned int loopcounter=0;
	unsigned MaxMinCounter=0;
	// General main loop (while in Autonomous or Tele mode)
	while (true) {
		
		//Slave 1 jaguar per axle to the other jaguar
		TopMasterVoltage = ShooterJagTopA.GetOutputVoltage();
		BottomMasterVoltage = ShooterJagBottomA.GetOutputVoltage();
		ShooterJagTopB.Set(TopMasterVoltage);
		ShooterJagBottomB.Set(BottomMasterVoltage);
		
		TopSpeedCur=ShooterJagTopA.GetSpeed();
		BottomSpeedCur=ShooterJagBottomA.GetSpeed();
		if(MaxMinCounter>=10){
			if(TopSpeedCur>TopSpeedHigh){
				TopSpeedHigh=TopSpeedCur;
			} else if(TopSpeedCur<TopSpeedLow){
				TopSpeedLow=TopSpeedCur;
			}
			if(BottomSpeedCur>BottomSpeedHigh){
				BottomSpeedHigh=BottomSpeedCur;
			} else if(BottomSpeedCur<BottomSpeedLow){
				BottomSpeedLow=BottomSpeedCur;
			}
		}
		MaxMinCounter++;
		//Set Speed
		 if(proxy->get(SHOOTER_TOP_SPEED_INCREASE, true)) {
			 ManualTopSpeed= ManualTopSpeed + 50;
			 printf("Top Increase\n");
		} else if(proxy->get(SHOOTER_TOP_SPEED_DECREASE, true)) {
			ManualTopSpeed = ManualTopSpeed - 50;
			printf("Top Decrease\n");
		}
		if(proxy->get(SHOOTER_BOTTOM_SPEED_INCREASE, true)) {
			printf("Bottom Increase\n");
			ManualBottomSpeed = ManualBottomSpeed + 50;
		} else if(proxy->get(SHOOTER_BOTTOM_SPEED_DECREASE, true)) {
			printf("Bottom Decrease\n");
			ManualBottomSpeed = ManualBottomSpeed - 50;
		}
		//printf("TOP: %4.1f Bottom: %4.1f\r", ShooterJagTopA.GetSpeed(), ShooterJagBottomA.GetSpeed());
		//printf("Speed: %f\r", Speed);
		//Press trigger to make motors go
#if PID
		if(proxy->get(SHOOTER_TRIGGER)) {
			TopSpeed = -(KEY_SPEED_TOP);
			BottomSpeed = KEY_SPEED_BOTTOM;	
		} else if (proxy->get(SHOOTER_MANUAL_TRIGGER)) {
			
			TopSpeed = ManualTopSpeed;
			BottomSpeed = ManualBottomSpeed;
		} else {
			MaxMinCounter=0;
			TopSpeed = 0;
			BottomSpeed = 0;
		}
		if(proxy->get("joy3b3")){
			TopSpeedHigh=TopSpeedCur;
			TopSpeedLow=TopSpeedCur;
			BottomSpeedHigh=BottomSpeedCur;
			BottomSpeedLow=BottomSpeedCur;
		}
#else 
		if (proxy->get("joy3b1")){
			TopSpeed = -1.5;
		} else {
			TopSpeed = 0;
		}
		if(proxy->get("joy3b2")){
			BottomSpeed = 4.25;
		} else {
			BottomSpeed = 0;
		}
#endif
		ShooterJagTopA.Set(TopSpeed);
		ShooterJagBottomA.Set(BottomSpeed);
#if PID
		if ((loopcounter%7)==0) {
			if((TopSpeed<-1) && (TopSpeed>1)) {
				if((TopMasterVoltage<=0.1) && (TopMasterVoltage>=-0.1)){
					printf("Top Dropped out\n");
					//Configure Top Jaguar A
					ShooterJagTopA.ConfigEncoderCodesPerRev(360);
					ShooterJagTopA.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
					ShooterJagTopA.ChangeControlMode(CANJaguar::kSpeed);
					ShooterJagTopA.SetPID(P,I,D);
					ShooterJagTopA.EnableControl(0);
					//Configure Top Jaguar B
					ShooterJagTopB.ChangeControlMode(CANJaguar::kVoltage);
					ShooterJagTopB.EnableControl();
				}
			} 
		}
		if((loopcounter%13)==0) {
			printf("Speed: %f Volt: %f\r", BottomSpeed, BottomMasterVoltage);
			if((BottomSpeed<-1) && (BottomSpeed >=1)) {
				if((BottomMasterVoltage<=0.1) && (BottomMasterVoltage>=-0.1)){
					printf("Bottom Dropped out\n");
					//Configure Bottom Jaguar A
					ShooterJagBottomA.ConfigEncoderCodesPerRev(360);
					ShooterJagBottomA.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
					ShooterJagBottomA.ChangeControlMode(CANJaguar::kSpeed);
					ShooterJagBottomA.SetPID(P,I,D);
					ShooterJagBottomA.EnableControl(0);
					//Configure Bottom Jaguar B
					ShooterJagBottomB.ChangeControlMode(CANJaguar::kVoltage);
					ShooterJagBottomB.EnableControl();
				}
			}
		}
		#endif
		loopcounter++;
		SmartDashboard::Log((-1*ManualTopSpeed), "Top Speed");
		SmartDashboard::Log(((-1*ManualTopSpeed)/4500), "Top Speed Percentage");
		SmartDashboard::Log(TopSpeedCur, "Actual Speed Top");
		SmartDashboard::Log(TopSpeedHigh, "Top High");
		SmartDashboard::Log(TopSpeedLow, "Top Low");
		SmartDashboard::Log(ManualBottomSpeed, "Bottom Speed");
		SmartDashboard::Log((ManualBottomSpeed/4500), "Bottom Speed Percentage");
		SmartDashboard::Log(BottomSpeedCur, "Actual Speed Bottom");
		SmartDashboard::Log(BottomSpeedHigh, "Bottom High");
		SmartDashboard::Log(BottomSpeedLow, "Bottom Low");
		// Make this match the declaraction above
		/*sl.PutOne(
				TopSpeed,
				BottomSpeed,
				ShooterJagTopA.GetSpeed(),
				ShooterJagBottomA.GetSpeed(),
				ShooterJagTopA.GetOutputVoltage(),//VoltageTopA,
				ShooterJagTopB.GetOutputVoltage(),//VoltageTopB,
				ShooterJagBottomA.GetOutputVoltage(),//VoltageBottomA,
				ShooterJagBottomB.GetOutputVoltage()//VoltageBottomB
				);*/
		// Wait for our next lap
		WaitForNextLoop();		
	}
	return (0);
	
};

