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
#include "proxy.h"

#define TPRINTF if(true) dprintf


float Ballistics(ParticleAnalysisReport* Target,int button, int distance)
{
	Proxy *proxy2;
	proxy2 = Proxy::getInstance();
	float targetheight = Target[TOP_MOST].center_mass_y;
	float targetheightr =((Target[TOP_MOST].center_mass_y)-(Target[RIGHT_MOST].center_mass_y)); //vertical distance from the middle of the image to the middle of the particle; parallel to the edge
	//TPRINTF(LOG_INFO, "targetheightr: %f", targetheightr);
	float targetheightl=(Target[TOP_MOST].center_mass_y-Target[LEFT_MOST].center_mass_y); //vertical distance from the middle of the image to the middle of the particle; parallel to the edge
	//TPRINTF(LOG_INFO, "targetheightl: %f", targetheightl);
	float normaldistance; //distance from closest point on the wall, i.e. the perpendicular
	float pdistance; //perpendicular to normal distance, along the wall to the hoop
	float angle; //angle of rotation of the robot, i.e. angle between line of sight and the wall
	float tangle; //angle the turret needs to turn after all caluclations are performed
	float calctargetwidth; //width of the vision target on the image if the robot was looking at it head on
	float particleheight=Target[BOTTOM_MOST].boundingRect.height; //height of the particle
	float realtargetwidth = Target[BOTTOM_MOST].boundingRect.width; //width of the vision target in the image
	float deltax; //diagonal distance to the hoop, i.e. line of sight
	float vo; //launch velocity, ft/s
	float rpm; //launch velocity converted into rpm
	float derivative; //derivative of equation of the parabola at point deltax
	float eangle; //entry angle
	float ratio;
	
	//This section claculates the distance from the target (parallel to the ground) and the turret angle
	TPRINTF(LOG_INFO, "distance: %i", distance);
	ratio=distance/targetheight;
	TPRINTF(LOG_INFO, "ratio: %f", ratio);
	/*normaldistance=(RATIO*targetheight);
	TPRINTF(LOG_INFO, "normaldistance: %f", normaldistance);	
	calctargetwidth=particleheight*24/18; //24/18 is ratio of width to height of vision target
	TPRINTF(LOG_INFO, "calctargetwidth: %f", calctargetwidth);
	angle=(PI/2)-(acos(realtargetwidth/calctargetwidth));
	TPRINTF(LOG_INFO, "angle: %f", (angle*180/PI));
	pdistance=normaldistance*(tan(angle));
	TPRINTF(LOG_INFO, "pdistance: %f", pdistance);
	deltax=sqrt(pow((normaldistance-HOOP),2)+pow(pdistance,2));
	deltax-=5/6;
	TPRINTF(LOG_INFO, "deltax: %f", deltax);
	tangle=(atan(pdistance/(normaldistance-HOOP))-angle);
	tangle=tangle*180/PI;
	//TPRINTF(LOG_INFO, "tangle: %f", tangle);
	
	
	
	//This section calculates the launch velocity needed (vo)
	vo=sqrt((GRAVITY*pow(deltax, 2))/(2*(pow(cos(LANGLE),2))*(deltax*tan(LANGLE)-TOPDELTAY))); //there is a notepad somewhere with calculations
	rpm=vo*60/(2*PI/3);
	//TPRINTF(LOG_INFO, "vo: %f", vo);
	TPRINTF(LOG_INFO, "rpm: %f", rpm);
	
	
	
	//this section figures out whether or not the ball can enter at the calulated trajectory
	derivative=(tan(LANGLE)-((GRAVITY*pow(deltax,2))/(pow((vo*cos(LANGLE)),2))));
	eangle=atan(derivative);
	//TPRINTF(LOG_INFO, "eangle: %f", eangle);
	if (eangle<(-27*180/PI))
	{
		//TPRINTF(LOG_INFO, "safe to launch");
	}
	else
	{
		//TPRINTF(LOG_INFO, "too close");
	}
	
	
	
	//sending values to the proxy
	proxy2->set("turret_angle",tangle);
	proxy2->set("initial_velocity",vo);*/
	return (0);
}
