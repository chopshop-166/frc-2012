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

#define RATIO 3 //placeholder
#define GRAVITY (32.17) //ft/s^2
//BEFORE: (32.17) //ft/s^2
#define PI 3.14159265358979
#define TOPDELTAY (71.25/12) //vertical distance from launcher to top hoop
//Note: the height of the robot from the wheels to the camera is 43 inches.
#define LANGLE (PI/4) //launch angle = 45 degrees
#define HOOP (15/12)//distance between hoop and vision target



float Ballistics(ParticleAnalysisReport* Target,int button, int distance);

