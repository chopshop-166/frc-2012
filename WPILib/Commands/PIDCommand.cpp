/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Commands/PIDCommand.h"

#include "SmartDashboard/SendablePIDController.h"
#include "float.h"

// XXX max and min are not used?

PIDCommand::PIDOutputImpl::PIDOutputImpl(PIDCommand *pidcommand)
{
	command = pidcommand;
}

void PIDCommand::PIDOutputImpl::PIDWrite(float output)
{
	command->UsePIDOutput(output);
}

PIDCommand::PIDSourceImpl::PIDSourceImpl(PIDCommand *pidcommand)
{
	command = pidcommand;
}

double PIDCommand::PIDSourceImpl::PIDGet()
{
	return command->ReturnPIDInput();
}

PIDCommand::PIDCommand(const char *name, double p, double i, double d) :
	Command(name)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output);
}

PIDCommand::PIDCommand(const char *name, double p, double i, double d, double period) :
	Command(name)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output, period);
}

PIDCommand::PIDCommand(double p, double i, double d)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output);
}

PIDCommand::PIDCommand(double p, double i, double d, double period)
{
	m_max = DBL_MAX;
	m_min = DBL_MIN;
	PIDSource *source = new PIDSourceImpl(this);
	PIDOutput *output = new PIDOutputImpl(this);
	m_controller = new SendablePIDController(p, i, d, source, output, period);
}

PIDController *PIDCommand::GetPIDController()
{
	return m_controller;
}

void PIDCommand::_Initialize()
{
	m_controller->Enable();
}

void PIDCommand::_End()
{
	m_controller->Disable();
}

void PIDCommand::_Interrupted()
{
	_End();
}

void PIDCommand::SetSetpointRelative(double deltaSetpoint)
{
	SetSetpoint(GetSetpoint() + deltaSetpoint);
}

void PIDCommand::SetSetpoint(double setpoint)
{
	m_controller->SetSetpoint(setpoint);
}

double PIDCommand::GetSetpoint()
{
	return m_controller->GetSetpoint();
}

double PIDCommand::GetPosition()
{
	return ReturnPIDInput();
}

void PIDCommand::SetSetpointRange(double a, double b)
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

std::string PIDCommand::GetType()
{
	return "PIDCommand";
}

NetworkTable *PIDCommand::GetControllerTable()
{
	return m_controller->GetTable();
}

