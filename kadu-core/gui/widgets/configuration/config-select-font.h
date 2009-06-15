/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_SELECT_FONT_H
#define CONFIG_SELECT_FONT_H

#include "gui/widgets/configuration/config-widget-value.h"
#include "gui/widgets/select-font.h"
#include "configuration/configuration-window-data-manager.h"

class QLabel;

class ConfigGroupBox;

/**
	&lt;select-font caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigSelectFont : public SelectFont, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox0, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSelectFont();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif
