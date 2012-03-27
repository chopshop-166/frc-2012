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
#define SHOOTER_CYCLE_TIME (50) // 10ms
#define PID (0)

#define KEY_SPEED_TOP (-1.5)
#define KEY_SPEED_BOTTOM (4.25)
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
	
	float BackspinFactor; // assign a value to this to differentiate top and bottom speed
	//Jags for the shooter
	CANJaguar ShooterJagTopA;
	CANJaguar ShooterJagTopB; 
	CANJaguar ShooterJagBottomA;
	CANJaguar ShooterJagBottomB;
	float P;//Proportianl value
	float I;//Integral value
	float D;//Derivative value
	float TopSpeed;
	float BottomSpeed;
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};
