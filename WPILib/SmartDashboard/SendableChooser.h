/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef __SENDABLE_CHOOSER_H__
#define __SENDABLE_CHOOSER_H__

#include "SmartDashboard/SmartDashboardData.h"
#include <map>
#include <string>

class NetworkTable;

class SendableChooser : public SmartDashboardData
{
public:
	SendableChooser();
	virtual ~SendableChooser() {};

	void AddObject(const char *name, void *object);
	void AddDefault(const char *name, void *object);
	void *GetSelected();

	virtual std::string GetType() {return "String Chooser";}
	virtual NetworkTable *GetTable() {return m_table;}

private:
	void *m_defaultChoice;
	std::map<std::string, void *> m_choices;
	std::map<void *, std::string> m_ids;
	NetworkTable *m_table;
	int m_count;
};

#endif
