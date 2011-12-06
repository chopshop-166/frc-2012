/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __PID_SUBSYSTEM_H__
#define __PID_SUBSYSTEM_H__

#include "Commands/Subsystem.h"
#include "PIDController.h"
#include "PIDSource.h"
#include "PIDOutput.h"

class NetworkTable;
class SendablePIDController;

class PIDSubsystem : public Subsystem
{
public:
	PIDSubsystem(const char *name, double p, double i, double d);
	PIDSubsystem(const char *name, double p, double i, double d, double period);
	PIDSubsystem(double p, double i, double d);
	PIDSubsystem(double p, double i, double d, double period);
	virtual ~PIDSubsystem();
	
	void Enable();
	void Disable();
	NetworkTable *GetControllerTable();

	virtual std::string GetType();

protected:
	PIDController *GetPIDController();
	void SetSetpoint(double setpoint);
	void SetSetpointRelative(double deltaSetpoint);
	double GetSetpoint();
	double GetPosition();
	void SetSetpointRange(double a, double b);
	virtual double ReturnPIDInput() = 0;
	virtual void UsePIDOutput(double output) = 0;

private:
	// TODO: Eliminate the classes
	class PIDOutputImpl : public PIDOutput {
	public:
		PIDOutputImpl(PIDSubsystem *pidsubsystem);
		void PIDWrite(float output);
	
	private:
		PIDSubsystem *subsystem;
	};
	class PIDSourceImpl : public PIDSource {
	public:
		PIDSourceImpl(PIDSubsystem *pidsubsystem);
		double PIDGet();
		
	private:
		PIDSubsystem *subsystem;
	};

	/** The max setpoint value */
	double m_max;
	/** The min setpoint value */
	double m_min;
	SendablePIDController *m_controller;
};

#endif

