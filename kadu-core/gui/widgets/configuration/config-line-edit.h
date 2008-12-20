/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_LINE_EDIT_H
#define CONFIG_LINE_EDIT_H

#include <QtGui/QLineEdit>

#include "gui/widgets/configuration/config-widget-value.h"
#include "configuration/configuration-window-data-manager.h"

class QLabel;
class ConfigGroupBox;

/**
	&lt;line-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigLineEdit : public QLineEdit, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigLineEdit();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif