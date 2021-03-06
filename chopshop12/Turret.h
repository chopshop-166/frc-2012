/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TaskTurret.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Turret header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/
	
#pragma once
	
#include "WPILib.h"
#include "Robot.h"
#include "Defines.h"


// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define TURRET_CYCLE_TIME (100) // 100ms
#define THRESHOLD		(5)
#define CENTERVOLTAGE	(4.3)
#define STRINGEXCESS	(3.59)
#define TURRETANGLE		(.34)
#define DEADBAND		(.1)
#define MAX_SPEED		(2) //the fastest speed at which the turret will move
// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
class Turret166 : public Team166Task
{	
	
public:
	
	// task constructor
	Turret166(void);
	
	// task destructor
	virtual ~Turret166(void);
	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	CANJaguar rotateturret;
	float rspeed;
	float volt;
	float angle;
	float CameraX;
	float centeroffset;
	// Any variables that the task has as members go here
	// <<CHANGEME>>
};	
