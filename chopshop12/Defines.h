/*******************************************************************************
*  Project   		: Chopshop12
*  File Name  		: Defines.h    
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Robot code header which contains defines for global usage
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#pragma once

// <<CHANGEME>>
// Place any #defines for things you want to enable in multiple tasks, simply, here
// For example,
// #define DRIVER_JOYSTICK (1)

//CONTROL Defines
#define DRIVE_1_JOYSTICK_Y				("Joy1Y")
#define DRIVE_1_JOYSTICK_X				("Joy1X")
#define DRIVE_2_JOYSTICK_Y				("Joy2Y")
#define BM_BUTTON						("joy1b2")
#define BM_BUTTON_N						("joy1b2n")
#define DRIVE_OPPOSITE_BUTTON_LEFT		("joy1b3")
#define DRIVE_OPPOSITE_BUTTON_N_LEFT	("joy1b3n")
#define DRIVE_OPPOSITE_BUTTON_RIGHT		("joy2b3")
#define DRIVE_OPPOSITE_BUTTON_N_RIGHT	("joy2b3n")
#define DRIVE_CREEP_BUTTON_LEFT			("joy1b4")
#define DRIVE_CREEP_BUTTON_N_LEFT 		("joy1b4n")
#define DRIVE_CREEP_BUTTON_RIGHT		("joy2b4")
#define DRIVE_CREEP_BUTTON_N_RIGHT		("joy2b4n")
#define SHOOTER_TRIGGER					("joy3b1")

//JAGUAR ID's
#define SHOOTER_JAG_TOP_A		(3)
#define SHOOTER_JAG_TOP_B		(4)
#define SHOOTER_JAG_BOTTOM_A	(5)
#define SHOOTER_JAG_BOTTOM_B	(6)
#define TURRET_JAG				(7)

//VICTORS ID's
#define DRIVE_FRONT_LEFT		(1)
#define DRIVE_FRONT_RIGHT		(2)
#define DRIVE_REAR_LEFT			(3)
#define DRIVE_REAR_RIGHT		(4)
#define BALL_JAGUAR				(6)
#define BALL_COLLECTOR			(7)
#define BRIDGE_MANIPULATOR		(8)
#define BALL_FEED				(9)

//DIGITAL INPUTS
#define BRIDGE_TOP_LIMIT		(2)
#define BRIDGE_BOTTOM_LIMIT		(1)
#define BALL_LOCATION_0			(3)
#define BALL_LOCATION_1			(4)
#define BALL_LOCATION_2			(5)
#define BALL_LOCATION_3			(6)
#define INCLINOMETER_CHANNEL_A  (7)
#define INCLINOMETER_CHANNEL_B  (8)

//ANALOG INPUTS
#define TURRET_POT			(1)
