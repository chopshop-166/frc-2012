/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskTemplate.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Template header file for tasks, with template functions
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
#define TEMPLATE_CYCLE_TIME (10) // 10ms

// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
class Shooter : public Team166Task
{
	
public:
	
	// task constructor
	Shooter(void);

	// task destructor
	virtual ~Shooter(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	
	
	float speed1; //new speed for bottom
	float speed2; //new speed for bottom
	
	float CurrentSpeed; //speed that motor is currently running at
	float BaskspinFactor; // assign a value to this to differentiate top and bottom speed
	CANJaguar ShooterJagTopA, ShooterJagTopB, ShooterJagBottomA, ShooterJagBottomB;
	
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
