/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SENDABLE_PID_CONTROLLER_H__
#define __SENDABLE_PID_CONTROLLER_H__

#include "NetworkTables/NetworkTableChangeListener.h"
#include "PIDController.h"
#include "SmartDashboard/SmartDashboardData.h"

class NetworkTable;

class SendablePIDController : public PIDController, public SmartDashboardData, public NetworkTableChangeListener
{
public:
	SendablePIDController(double p, double i, double d, PIDSource *source, PIDOutput *output);
	SendablePIDController(double p, double i, double d, PIDSource *source, PIDOutput *output, double period);
	virtual ~SendablePIDController();

	void SetPID(double p, double i, double d); 
	void Enable();
	void Disable();

	virtual std::string GetType() {return "PIDController";}
	virtual NetworkTable *GetTable();

	virtual void ValueChanged(NetworkTable *table, const char *name, NetworkTables_Types type);
	virtual void ValueConfirmed(NetworkTable *table, const char *name, NetworkTables_Types type) {}

private:
	NetworkTable* m_table;
};

#endif
