/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_LABEL_H
#define CONFIG_LABEL_H

#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-widget.h"
#include "configuration/configuration-window-data-manager.h"

class ConfigGroupBox;

/**
	&lt;label caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigLabel : public QLabel, public ConfigWidget
{
protected:
	virtual void createWidgets();

public:
	ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	ConfigLabel(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigLabel() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

#endif
