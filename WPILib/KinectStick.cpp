/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "KinectStick.h"
#include "Joystick.h"
#include "Utility.h"
#include "NetworkCommunication/FRCComm.h"

/**
 * Kinect joystick constructor
 * @param id value is either 1 or 2 for the left or right joystick decoded from
 * gestures interpreted by the Kinect server on the Driver Station computer.
 */
KinectStick::KinectStick(int id)
{
	wpi_assert(id == 1 || id == 2);
	m_id = id;
}

/**
 * Get dynamic data from the driver station buffer
 */
void KinectStick::GetData()
{
	int retVal = getDynamicControlData(kJoystickBundleID, sticks.data, sizeof(sticks.data), 5);
	if (retVal == 0)
	{
		wpi_assert(sticks.formatted.size == sizeof(sticks.data) - 1);
	}
}

/**
 * Get the X joystick value
 * This is not supported in the default gestures and always returns 0
 */
float KinectStick::GetX(JoystickHand hand)
{
	return 0.0;
}

/**
 * Convert an 8 bit joystick value to a floating point (-1,1) value
 * @param value The 8 bit raw joystick value returned from the driver station
 */
float KinectStick::convertRawToFloat(INT8 value)
{
	float result;
	if (value < 0)
		result = ((float) value) / 128.0;
	else
		result = ((float) value) / 127.0;
	wpi_assert(result <= 1.0 && result >= -1.0);
	if (result > 1.0)
		result = 1.0;
	else if (result < -1.0)
		result = -1.0;
	return result;
}

/**
 * Get the joystick Y value
 * @param hand
 * @returns The floating point representation of the Y value
 */
float KinectStick::GetY(JoystickHand hand)
{
	GetData();
	float value = convertRawToFloat(sticks.formatted.rawSticks[m_id - 1].axis[Joystick::kYAxis]);
	return value;
}

/**
 * Get the Z joystick value
 * This is not supported in the default gestures and always returns 0
 */
float KinectStick::GetZ()
{
	return 0;
}

/**
 * Get the Twist joystick value
 * This is not supported in the default gestures and always returns 0
 */
float KinectStick::GetTwist()
{
	return 0;
}

/**
 * Get the Throttle joystick value
 * This is not supported in the default gestures and always returns 0
 */
float KinectStick::GetThrottle()
{
	return 0;
}

/**
 * Get the joystick axis value by axis number
 * This is only supported for X in the default gestures and all others always returns 0
 */
float KinectStick::GetRawAxis(UINT32 axis)
{
	if (axis == Joystick::kYAxis)
		return GetY();
	else
		return 0;
}

/**
 * Get the trigger value for the joystick
 * @param hand
 * @returns true if the triger is pressed
 */
bool KinectStick::GetTrigger(JoystickHand hand)
{
	GetData();
	return (sticks.formatted.rawSticks[m_id - 1].buttons & kTriggerMask) != 0;
}

/**
 * Get the top button value for the joystick
 * @param hand
 * @returns true if the top button is pressed
 */
bool KinectStick::GetTop(JoystickHand hand)
{
	GetData();
	return (sticks.formatted.rawSticks[m_id - 1].buttons & kTopMask) != 0;
}

bool KinectStick::GetBumper(JoystickHand hand)
{
	GetData();
	return (sticks.formatted.rawSticks[m_id - 1].buttons & 4) != 0;
}

bool KinectStick::GetRawButton(UINT32 button)
{
	GetData();
	return (sticks.formatted.rawSticks[m_id - 1].buttons & (1 << button)) != 0;
}
