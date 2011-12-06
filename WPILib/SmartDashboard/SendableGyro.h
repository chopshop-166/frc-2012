/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SENDABLE_GYRO_H__
#define __SENDABLE_GYRO_H__

#include "Gyro.h"
#include "NetworkTables/NetworkTableChangeListener.h"
#include "Notifier.h"
#include "SmartDashboard/SmartDashboardData.h"

class NetworkTable;

class SendableGyro : public Gyro, public SmartDashboardData, public NetworkTableChangeListener
{
public:
	SendableGyro(UINT8 moduleNumber, UINT32 channel);
	SendableGyro(UINT32 channel);
	SendableGyro(AnalogChannel* channel);
	virtual ~SendableGyro();

	double GetAngle();
	void Reset();
	void SetUpdatePeriod(double period);
	double GetUpdatePeriod();
	void ResetToAngle(double angle);

	virtual std::string GetType() {return "Gyro";}
	virtual NetworkTable *GetTable();

	// for dealing with the "thread"
private:
	virtual void ValueChanged(NetworkTable *table, const char *name, NetworkTables_Types type);
	virtual void ValueConfirmed(NetworkTable *table, const char *name, NetworkTables_Types type) {}

	static void TimerUpdate(void *gyro);

	Notifier *m_notifier;
	double m_offset;
	double m_period;
	NetworkTable *m_table;
};

#endif
