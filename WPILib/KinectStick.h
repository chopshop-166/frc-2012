/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KINECT_STICK_H
#define KINECT_STICK_H

#include "GenericHID.h"
#include "ErrorBase.h"

class KinectStick : public GenericHID, public ErrorBase {
public:
	explicit KinectStick(int id);
	virtual float GetX(JoystickHand hand = kRightHand);
	virtual float GetY(JoystickHand hand = kRightHand);
	virtual float GetZ();
	virtual float GetTwist();
	virtual float GetThrottle();
	virtual float GetRawAxis(UINT32 axis);

	virtual bool GetTrigger(JoystickHand hand = kRightHand);
	virtual bool GetTop(JoystickHand hand = kRightHand);
	virtual bool GetBumper(JoystickHand hand = kRightHand);
	virtual bool GetRawButton(UINT32 button);
private:
	static const int kJoystickBundleID = 24;
	static const int kTriggerMask = 1;
	static const int kTopMask = 2;

	void GetData();
	float convertRawToFloat(INT8 charValue);
	int m_id;
	union {
		struct {
			UINT8 size;
			UINT8 id;
			struct {
				unsigned char axis[6];
				unsigned short buttons;
			} rawSticks[2];
		} formatted;
		char data[18];
	} sticks;
};

#endif

