/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: BridgeBalance.h     
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Code for balancing on the bridge
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
#define BridgeBalance_CYCLE_TIME (10) // 10ms
#define BALANCEDANGLE (25)
// Rename this, too, or you'll run into collisions
// <<CHANGEME>>
class BridgeBalance166 : public Team166Task
{
	
public:
	
	// task constructor
	BridgeBalance166(void);

	// task destructor
	virtual ~BridgeBalance166(void);

	// Main function of the task
	virtual int Main(int a2, int a3, int a4, int a5,
			int a6, int a7, int a8, int a9, int a10);
	
private:
	//Declare Proxy and Robot handles
	Proxy *proxy;				// Handle to proxy
	Robot *lHandle;            // Local handle
	// Any variables that the task has as members go here
	Encoder Balance; //the inclinometer for bridge balancing
	int TempAngle; //a variable to hold the angle of the inclinometer
	// <<CHANGEME>>
};
