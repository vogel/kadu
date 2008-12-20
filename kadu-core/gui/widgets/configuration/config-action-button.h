/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_ACTION_BUTTON_H
#define CONFIG_ACTION_BUTTON_H

#include <QtGui/QPushButton>
#include "gui/widgets/configuration/config-widget.h"
#include "configuration/configuration-window-data-manager.h"

class ConfigGroupBox;

/**
	&lt;action-button caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigActionButton : public QPushButton, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigActionButton() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

#endif
