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
	Timer shooterTimer, driveTimer, pauseTimer;
	proxy->set("joy1T", -1);
	while( lHandle->IsAutonomous() ) {
		
		proxy->set("joy3b10", 1);
		
		//printf("State: %d \n", int(state));
		switch(state){
			case INIT:
				//Lower Bridge Manipulator
				//proxy->set(BM_BUTTON, 1);
				state = IS_ALIGNING;
				break;
			case IS_ALIGNING:
				//Stop holding bridge manipulator button
				//if(proxy->get(BM_BUTTON))
				//	proxy->set(BM_BUTTON, 0);
				if(proxy->get("CameraX")<.1&&proxy->get("CameraX")>-.1){
					proxy->set("joy3b10", 0);
					state = CHECK_BALL;
				}
				break;
			case CHECK_BALL:
				if(int(proxy->get("BallCount"))>0){
					prevCount = int(proxy->get("BallCount"));
					state = LOAD_N_SHOOT;
				}
				else
					state = DONE;
				break;
			case LOAD_N_SHOOT:
				
				//start shooting, loading, and timer
				if(shooterTimer.Get()==0)	//start time 3 second timer
					shooterTimer.Start();
				if(proxy->get(SHOOTER_TRIGGER)==0)
					proxy->set(SHOOTER_TRIGGER, 1);	//start shooting
				
				//when the 3 seconds are up, stop timer and shooter
				if(shooterTimer.Get()>=2){
					shooterTimer.Stop();
					proxy->set(SHOOTER_TRIGGER, 0);
					shooterTimer.Reset();
					state = PAUSE_ONE_SEC;
				}
				break;
			case PAUSE_ONE_SEC:
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
