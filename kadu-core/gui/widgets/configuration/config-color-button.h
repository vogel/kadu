/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_COLOR_BUTTON_H
#define CONFIG_COLOR_BUTTON_H

#include "gui/widgets/configuration/config-widget-value.h"
#include "configuration/configuration-window-data-manager.h"
#include "color_button.h"

class QLabel;
class ConfigGroupBox;

/**
	&lt;colob-button caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigColorButton : public ColorButton, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigColorButton();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif