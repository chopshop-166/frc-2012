/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/PIDSubsystem.h"
#include "SmartDashboard/SendablePIDController.h"
#include "float.h"

// XXX max and min are not used?

PIDSubsystem::PIDOutputImpl::PIDOutputImpl(PIDSubsystem *pidsubsystem)
{
	subsystem = pidsubsystem;
}

void PIDSubsystem::PIDOutputImpl::PIDWrite(float output)
{
	subsystem->UsePIDOutput(output);
}

PIDSubsystem::PIDSourceImpl::PIDSourceImpl(PIDSubsystem *pidsubsystem)
{
	subsystem = pidsubsystem;
}

double PIDSubsystem::PIDSourceImpl::PIDGet()
{
	return subsystem->ReturnPIDInput();
}

PIDSubsystem::PIDSubsystem(const char *name, double p, double i, double d) :
	Subsystem(name)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output);
}

PIDSubsystem::PIDSubsystem(const char *name, double p, double i, double d,
	double period) :
	Subsystem(name)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output, period);
}

PIDSubsystem::PIDSubsystem(double p, double i, double d) :
	Subsystem("PIDSubsystem")
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output);
}

PIDSubsystem::PIDSubsystem(double p, double i, double d, double period) :
	Subsystem("PIDSubsystem")
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output, period);
}

void PIDSubsystem::Enable()
{
	m_controller->Enable();
}

void PIDSubsystem::Disable()
{
	m_controller->Disable();
}

PIDController *PIDSubsystem::GetPIDController()
{
	return m_controller;
}
void PIDSubsystem::SetSetpointRelative(double deltaSetpoint)
{
	SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDSubsystem::SetSetpoint(double setpoint)
{
	m_controller->SetSetpoint(setpoint);
}

double PIDSubsystem::GetSetpoint()
{
	return m_controller->GetSetpoint();
}

double PIDSubsystem::GetPosition()
{
	return ReturnPIDInput();
}

void PIDSubsystem::SetSetpointRange(double a, double b)
{
	if (a <= b)
	{
		m_min = a;
		m_max = b;
	}
	else
	{
		m_min = b;
		m_max = a;
	}
}

std::string PIDSubsystem::GetType()
{
	return "PIDSubsystem";
}

NetworkTable *PIDSubsystem::GetControllerTable()
{
	return m_controller->GetTable();
}
