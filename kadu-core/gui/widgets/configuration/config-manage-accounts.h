/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_MANAGE_ACCOUNTS_H
#define CONFIG_MANAGE_ACCOUNTS_H

#include "gui/widgets/configuration/config-widget.h"
#include "configuration/widgets/manage-accounts.h"

class QLabel;
class ConfigGroupBox;
class ConfigurationWindowDataManager;

/**
	&lt;manage-accounts caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigManageAccounts : public ManageAccounts, public ConfigWidget
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigManageAccounts(const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, const char *name);
	ConfigManageAccounts(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigManageAccounts();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif
