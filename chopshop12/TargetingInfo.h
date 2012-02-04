/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TargetingInfo.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: TargetingInfo header file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "math.h"
#include "Target2.h"

//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
// You should rename this when you copy it into a new file
// <<CHANGEME>>
#define TARGETINGINFO_CYCLE_TIME (500) // 10ms
#define DISTANCECALIBRATION 4 //THIS IS TEMPORARY! NOT THE ACTUAL VALUE!distance the robot is from the hoop when the top vision target is at the very top of the image
#define GRAVITY 32.17 //ft/s^2
#define PI 3.14159265
#define TOPDELTAY 5 //THIS IS TEMPORARY! NOT THE ACTUAL VALUE!vertical distance from launcher to top hoop
#define LANGLE 0.872664626 //launch angle = 50 degrees
// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
class TargetingInfo166 : public Team166Task
{
	
public:
	
	// task constructor
	TargetingInfo166(void);

	// task destructor
	virtual ~TargetingInfo166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	// Any variables that the task has as members go here
	float imageheight; //obtain from M's function
	float targetheight; //vertical distance from the middle of the image to the middle of the particle; parallel to the edge
	float distance; //distance from closest point on the wall, i.e. the perpendicular
	float angle; //angle of rotation of the robot, i.e. angle between line of sight and the wall
	float calctargetwidth; //width of the vision target on the image if the robot was looking at it head on
	float realtargetwidth; //width of the vision target in the image
	float deltax; //diagonal distance to the hoop, i.e. line of sight
	float vo; //launch velocity, ft/s
	float time; //time in the air in seconds
	float vox; //initial and final horizontal velocity component
	float voy; //initial vertical velocity component
	float vfy; //final vertical velocity component
	float eangle; //entry angle
};
