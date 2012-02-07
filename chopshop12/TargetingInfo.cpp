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

float Ballistics(ParticleAnalysisReport top,ParticleAnalysisReport left,ParticleAnalysisReport right,ParticleAnalysisReport bottom,int button)
{
	//This section claculates the distance from the target (parallel to the ground)
	normaldistance=(DISTANCECALIBRATION*imageheight/targetheight);
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
