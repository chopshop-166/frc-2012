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
#include "nivision.h"
#include "proxy.h"

float Ballistics(ParticleAnalysisReport top,ParticleAnalysisReport left,ParticleAnalysisReport right,ParticleAnalysisReport bottom,int button)
{
	float imageheight; //obtain from M's function
	float targetheight; //vertical distance from the middle of the image to the middle of the particle; parallel to the edge
	float normaldistance; //distance from closest point on the wall, i.e. the perpendicular
	float pdistance; //perpendicular to normal distance, along the wall to the hoop
	float angle; //angle of rotation of the robot, i.e. angle between line of sight and the wall
	float tangle; //angle the turret needs to turn after all caluclations are performed
	float calctargetwidth; //width of the vision target on the image if the robot was looking at it head on
	float realtargetwidth; //width of the vision target in the image
	float deltax; //diagonal distance to the hoop, i.e. line of sight
	float vo; //launch velocity, ft/s
	float atime; //time in the air in seconds
	float vox; //initial and final horizontal velocity component
	float voy; //initial vertical velocity component
	float vfy; //final vertical velocity component
	float eangle; //entry angle
	
	//This section claculates the distance from the target (parallel to the ground) and the turret angle
	normaldistance=(DISTANCECALIBRATION*imageheight/targetheight);
	calctargetwidth=targetheight*24/18; //24/18 is ratio of width to height of vision target
	realtargetwidth=54;//TEMPORARY!!!!!!!
	angle=(PI/2)-(acos(realtargetwidth/calctargetwidth));
	pdistance=normaldistance*(tan(angle));
	deltax=sqrt(pow((normaldistance-HOOP),2)+pow(pdistance,2));
	tangle=(atan(pdistance/(normaldistance-HOOP))-angle);
	tangle=tangle*180/PI;
	
	
	
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
	
	
	
	//sending values to the proxy
	//proxy->set("turret_angle",tangle);
	//proxy->set("initial_velocity",vo);
	return (0);
}
