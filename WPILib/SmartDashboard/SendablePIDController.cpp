/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "SmartDashboard/SendablePIDController.h"

#include "NetworkTables/NetworkTable.h"

SendablePIDController::SendablePIDController(double p, double i, double d,
	PIDSource* source, PIDOutput* output) :
	PIDController(p, i, d, source, output)
{
}

SendablePIDController::SendablePIDController(double p, double i, double d,
	PIDSource* source, PIDOutput* output, double period) :
	PIDController(p, i, d, source, output, period)
{
}

SendablePIDController::~SendablePIDController()
{
	if (m_table != NULL)
		m_table->RemoveChangeListenerAny(this);
}

void SendablePIDController::SetPID(double p, double i, double d)
{
	PIDController::SetPID(p, i, d);

	if (m_table != NULL)
	{
		m_table->PutDouble("p", p);
		m_table->PutDouble("i", i);
		m_table->PutDouble("d", d);
	}
}

void SendablePIDController::Enable()
{
	PIDController::Enable();

	if (m_table != NULL)
		m_table->PutBoolean("enabled", true);
}

void SendablePIDController::Disable()
{
	PIDController::Disable();

	if (m_table != NULL)
		m_table->PutBoolean("enabled", false);
}

NetworkTable* SendablePIDController::GetTable()
{
	if (m_table == NULL)
	{
		m_table = new NetworkTable();

		m_table->PutDouble("p", GetP());
		m_table->PutDouble("i", GetI());
		m_table->PutDouble("d", GetD());
		m_table->PutDouble("setpoint", GetSetpoint());
		m_table->PutBoolean("enabled", IsEnabled());

		m_table->AddChangeListenerAny(this);
	}
	return m_table;
}

void SendablePIDController::ValueChanged(NetworkTable *table, const char *name, NetworkTables_Types type)
{
	if (strcmp(name, "p") == 0 || strcmp(name, "i") == 0 || strcmp(name, "d") == 0)
	{
		PIDController::SetPID(table->GetDouble("p"), table->GetDouble("i"),
			table->GetDouble("d"));
	}
	else if (strcmp(name, "setpoint") == 0)
	{
		PIDController::SetSetpoint(table->GetDouble("setpoint"));
	}
	else if (strcmp(name, "enabled") == 0)
	{
		if (table->GetBoolean("enabled"))
			PIDController::Enable();
		else
			PIDController::Disable();
	}
}
