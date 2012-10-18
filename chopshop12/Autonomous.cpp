/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: Autonomous.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Robot code to execute in autonomous mode
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "Robot.h"
#include "Autonomous.h"

// To locally enable debug printing: set true, to disable false
#define DPRINTF if(false)dprintf

AutonomousTask::AutonomousTask() {
	// Create handles for proxy and robot
	Robot *lHandle;
	Proxy *proxy;
	
	// Register robot handle
	while( !(lHandle = Robot::getInstance()) && !( lHandle->IsAutonomous() ) ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	// Register proxy handle
	while( (proxy = Proxy::getInstance()) == NULL ) {
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	state = INIT;
	int prevCount = 0;
	Timer shooterTimer, driveTimer, pauseTimer, failAlignTimer;
	proxy->set("joy1T", -1);
	while( lHandle->IsAutonomous() ) {
		
		proxy->set(TURRET_BTN_AUTO, 1);
		
		//printf("State: %d \n", int(state));
		switch(state){
			case INIT:
				lHandle->DriverStationDisplay("We are Initializing");
				//Lower Bridge Manipulator
				//proxy->set(BM_BUTTON, 1);
				proxy->set(SHOOTER_TRIGGER, 1);
				state = IS_ALIGNING;
				break;
			case IS_ALIGNING:
				lHandle->DriverStationDisplay("We are Aligning");
				//Stop holding bridge manipulator button
				//if(proxy->get(BM_BUTTON))
				//	proxy->set(BM_BUTTON, 0);
				if(failAlignTimer.Get()==0)	//incase the camera fails, start a 3 second timer
					failAlignTimer.Start();
				if((failAlignTimer.Get()>=3)){	//after 3 seconds of camera failing to align, continue
					failAlignTimer.Stop();
					failAlignTimer.Reset();
					proxy->set(TURRET_BTN_AUTO, 0);
					state = CHECK_BALL;
				}
				break;
			case CHECK_BALL:
				lHandle->DriverStationDisplay("We are Checking Ball %d", int(proxy->get("BallCount")));
				if(int(proxy->get("BallCount"))>0){
					prevCount = int(proxy->get("BallCount"));
					state = LOAD_N_SHOOT;
				}
				else
					state = DONE;
				break;
			case LOAD_N_SHOOT:
				lHandle->DriverStationDisplay("We are Shooting");
				//start shooting, loading, and timer
				if(shooterTimer.Get()==0)	//start time 3 second timer
					shooterTimer.Start();
				if(proxy->get(BALLFEED_MANUAL)==0)
					proxy->set(BALLFEED_MANUAL, 1);	//start shooting
				
				//when the 3 seconds are up, stop timer and shooter
				if(shooterTimer.Get()>=1){
					shooterTimer.Stop();
					proxy->set(BALLFEED_MANUAL, 0);
					shooterTimer.Reset();
					state = PAUSE_ONE_SEC;
				}
				break;
			case PAUSE_ONE_SEC:
				lHandle->DriverStationDisplay("We are Pausing");
				if(pauseTimer.Get()==0)
					pauseTimer.Start();
				else if(pauseTimer.Get()>=1){
					pauseTimer.Stop();
					pauseTimer.Reset();
					state = CHECK_BALL;
				}
				break;
			case START_DRIVE:
				proxy->set(DRIVE_1_JOYSTICK_Y, -.35);
				proxy->set(DRIVE_2_JOYSTICK_Y, -.35);
				driveTimer.Start();
				state = DO_DRIVE;
				break;
			case DO_DRIVE:
				if(driveTimer.Get()>=2){
					driveTimer.Stop();
					driveTimer.Reset();
					proxy->set(DRIVE_1_JOYSTICK_Y, 0);
					proxy->set(DRIVE_2_JOYSTICK_Y, 0);
					state = DONE;
				}
				break;
			case DONE:
				proxy->set(SHOOTER_TRIGGER, 0);
				break;
			default:
				printf("ERROR IF THIS IS SEEN");
				break;
		}
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
	
	state = INIT;
}
