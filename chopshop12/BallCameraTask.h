/*******************************************************************************
*  Project   		: Chopshop11
*  File Name  		: BallCameraTask.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  File Description	: Task to run the camera and tracking 
*  File Description	: Definition of BallCameraTask class and associated data
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

#include "WPILib.h"
#include "Robot.h"
#include "Proxy.h"
#include "Vision/AxisCamera.h"
#include "TrackAPI.h"
#include "CameraTask.h"


//
// This constant defines how often we want this task to run in the form
// of miliseconds. Max allowed time is 999 miliseconds.
//#define CAMERA_CYCLE_TIME (20) // ms
#define CAMERA_CYCLE_TIME (200) // ms

/** Private NI function needed to write to the VxWorks target */
IMAQ_FUNC int Priv_SetWriteFileAllowed(UINT32 enable); 

class BallCameraTask : public Team166Task 
{
	
public:
	
	// task constructor
	BallCameraTask(void);

	// task destructor
	virtual ~BallCameraTask(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
	// Search for Target
	bool FindTargets();
	
private:
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	AxisCamera &camera;
	static BallCameraTask *myHandle;      // handle to myself for static function
	
	//values to log
	double targetHAngle;
	double targetVAngle;
	double targetSize;
	int widestParticleIndex;
	ParticleAnalysisReport Biggest;
	int BallCameraTask::GetWidestParticle(Image* binaryImage, int* widestParticleIndex);
	double targetCenterNormalized;
	int BallCameraTask::ProcessImage(double* targetCenterNormalized);
	/*ddduuurrrrr...?
	 */
	int imageProcessResult;
	
	Range R_Range;
	Range B_Range;
	Range G_Range;
};
