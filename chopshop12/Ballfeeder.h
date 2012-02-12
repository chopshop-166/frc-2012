/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskBallFeeder.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: BallFeeder header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define BALLFEEDER_CYCLE_TIME (10) // 10ms
#define BALLFEED (-.25)
// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
class BallFeeder166 : public Team166Task
{
	
public:
	
	// task constructor
	BallFeeder166(void);

	// task destructor
	virtual ~BallFeeder166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	Victor BallFeed; //victor to control feeder
	DigitalInput BallLocation0;//digital ball sensor below fall feeder
	DigitalInput BallLocation1;//bottom digital ball sensor
	DigitalInput BallLocation2;//middle digital ball sensor
	DigitalInput BallLocation3;//top digital ball sensor
	int BallCount;//number of balls the robot has
	float feedspeed;
	enum {Stopped, CollectionStarted, Store1Ball, Store2Ball, Store3Ball} FeedState;
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
