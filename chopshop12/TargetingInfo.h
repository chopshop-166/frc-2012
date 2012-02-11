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

#include "math.h"

#define DISTANCECALIBRATION (1.7) //THIS IS TEMPORARY! NOT THE ACTUAL VALUE!distance the robot is from the hoop when the top vision target is at the very top of the image
#define GRAVITY (386.088583) //in/s^2
//BEFORE: (32.17) //ft/s^2
#define PI 3.14159265358979
#define TOPDELTAY 5 //THIS IS TEMPORARY! NOT THE ACTUAL VALUE!vertical distance from launcher to top hoop
//Note: the height of the robot from the wheels to the camera is 43 inches.
#define LANGLE 0.872664626 //launch angle = 50 degrees
#define HOOP 1//THIS IS TEMPORARY! distance between hoop and vision target


float Ballistics(ParticleAnalysisReport* Target,int button);

