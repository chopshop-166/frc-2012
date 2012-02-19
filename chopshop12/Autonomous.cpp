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
	Timer shooterTimer;
	
	while( lHandle->IsAutonomous() ) {
		
		switch(state){
			case INIT:
				state = IS_ALIGNING;
				// no break on purpose
			case IS_ALIGNING:
				if(proxy->get("CameraX")==0)
					state = CHECK_BALL;
				break;
			case CHECK_BALL:
				if(int(proxy->get("BallCount"))>0){
					prevCount = int(proxy->get("BallCount"));
					state = LOAD_N_SHOOT;
				}
				else
					state = START_DRIVE;
				break;
			case LOAD_N_SHOOT:
				
				//start shooting, loading, and timer
				if(shooterTimer.Get()==0)	//start time 3 second timer
					shooterTimer.Start();
				if(proxy->get("joy3b1")==0)
					proxy->set("joy3b1", 1);	//start shooting
				if(proxy->get("joy2b3")==0 && prevCount==(int(proxy->get("BallCount"))))
					proxy->set("joy2b3", 1);	//start loading
				
				
				// once its loaded fully, stop loading!
				else if(proxy->get("joy2b3")==1 && prevCount>(int(proxy->get("BallCount"))))
					proxy->set("joy2b3", 0);
				
				//when the 3 seconds are up, stop timer and shooter
				if(shooterTimer.Get()>=3){
					shooterTimer.Stop();
					proxy->set("joy2b3", 0);
					shooterTimer.Reset();
					state = CHECK_BALL;
				}
				break;
			case START_DRIVE:
				break;
			default:
				printf("ERROR IF THIS IS SEEN");
				break;
		}
		
		// This wait is required, it makes sure no task uses too much memory
		Wait(AUTONOMOUS_WAIT_TIME);
	}
}
