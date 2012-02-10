/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: TargetingInfo.cpp     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: TargetingInfo source file for tasks, with template functions
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/* Find & Replace "TargetingInfo" with the name you would like to give this task     */
/* Find & Replace "Testing" with the name you would like to give this task      */
/* Find & Replace "TargetingInfo" with the name you would like to give this task */
/*------------------------------------------------------------------------------*/

#include "WPILib.h"
#include "TargetingInfo.h"
#include "CameraTask.h"
#include "nivision.h"
#include "Target2.h"

#define TPRINTF if(true) dprintf
/*
 * 
 * MATT
 * 
 * I couldn't think of a way to test without editing your code. Sorry. If you want to see
 * the changes I made, just do a visual diff with revision 43. 
 * 
 */

float Ballistics(ParticleAnalysisReport* Target,int button)
{
	float imageheight= Target[TOP_MOST].imageWidth; //obtain from M's function
	float targetheight= abs((120)-(Target[TOP_MOST].center_mass_y)); //vertical distance from the middle of the image to the middle of the particle; parallel to the edge
	TPRINTF(LOG_INFO, "targetheight= %i", targetheight);
	float normaldistance; //distance from closest point on the wall, i.e. the perpendicular
	float angle; //angle of rotation of the robot, i.e. angle between line of sight and the wall
	float calctargetwidth; //width of the vision target on the image if the robot was looking at it head on
	float realtargetwidth = Target[TOP_MOST].boundingRect.width; //width of the vision target in the image
	float deltax; //diagonal distance to the hoop, i.e. line of sight
	float vo; //launch velocity, ft/s
	float atime; //time in the air in seconds
	float vox; //initial and final horizontal velocity component
	float voy; //initial vertical velocity component
	float vfy; //final vertical velocity component
	float eangle; //entry angle
	
	//This section claculates the distance from the target (parallel to the ground)
	normaldistance=(DISTANCECALIBRATION*imageheight/targetheight);
	TPRINTF(LOG_INFO, "normaldistance: %f", normaldistance);	
	calctargetwidth=targetheight*24/18; //24/18 is ratio of width to height of vision target
	realtargetwidth=54;//TEMPORARY!!!!!!!
	angle=(PI/2)-(acos(realtargetwidth/calctargetwidth));
	deltax=normaldistance/(sin(angle));
	
	
	
	
	//This section calculates the launch velocity needed (vo)
	vo=sqrt((GRAVITY*pow(deltax, 2))/(2*(pow(cos(LANGLE),2))*(deltax*tan(LANGLE)-TOPDELTAY))); //there is a notepad somewhere with calculations
	
	
	
	//this section figures out whether or not the ball can enter at the calulated trajectory
	vox=vo*cos(LANGLE);
	atime=deltax/vox;
	voy=vo*sin(LANGLE);
	vfy=voy-GRAVITY*atime;
	eangle=atan(vfy/vox);
	if (eangle>27*180/PI)
	{
		cout<<"Safe to launch";
	}
	else
	{
		cout<<"Not safe to launch";
	}
	return (0);
}
