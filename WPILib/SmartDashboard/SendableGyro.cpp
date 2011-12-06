/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendableGyro.h"

#include "NetworkTables/NetworkTable.h"
#include "WPIErrors.h"

static const double kDefaultTimeBetweenUpdates = 0.2;
static const char *kAngle = "angle";

SendableGyro::SendableGyro(UINT8 moduleNumber, UINT32 channel) :
	Gyro(moduleNumber, channel)
{
	m_offset = 0.0;
	m_period = kDefaultTimeBetweenUpdates;
	m_table = NULL;
	m_notifier = NULL;
}

SendableGyro::SendableGyro(UINT32 channel) :
	Gyro(channel)
{
	m_offset = 0.0;
	m_period = kDefaultTimeBetweenUpdates;
	m_table = NULL;
	m_notifier = NULL;
}

SendableGyro::SendableGyro(AnalogChannel* channel):  
	Gyro(channel)
{
	m_offset = 0.0;
	m_period = kDefaultTimeBetweenUpdates;
	m_table = NULL;
	m_notifier = NULL;
}

SendableGyro::~SendableGyro()
{
	if (m_table != NULL)
		m_table->RemoveChangeListener(kAngle, this);
}

double SendableGyro::GetAngle()
{
	return m_offset + Gyro::GetAngle();
}

void SendableGyro::Reset()
{
	m_offset = 0.0;
	Gyro::Reset();
}

void SendableGyro::SetUpdatePeriod(double period)
{
	if (period <= 0.0)
		wpi_setWPIErrorWithContext(ParameterOutOfRange, "period <= 0.0");
	else
		m_period = period;
}

double SendableGyro::GetUpdatePeriod()
{
	return m_period;
}

void SendableGyro::ResetToAngle(double angle)
{
	m_offset = angle;
	Gyro::Reset();
}

NetworkTable *SendableGyro::GetTable()
{
	if (m_table == NULL)
	{
		m_table = new NetworkTable();
		m_table->PutInt(kAngle, (int)GetAngle());
		m_table->AddChangeListener(kAngle, this);
		m_notifier = new Notifier(SendableGyro::TimerUpdate, this);
		m_notifier->StartSingle(m_period);
	}
	return m_table;
}

void SendableGyro::ValueChanged(NetworkTable *table, const char *name, NetworkTables_Types type)
{
	ResetToAngle(m_table->GetDouble(name));
}

void SendableGyro::TimerUpdate(void *gyro)
{
	SendableGyro *sg = (SendableGyro *)gyro;
	sg->m_table->PutInt(kAngle, (int)sg->GetAngle());
	sg->m_notifier->StartSingle(sg->m_period);
}
