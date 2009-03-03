/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_CHECK_BOX_H
#define CONFIG_CHECK_BOX_H

#include <QtGui/QCheckBox>

#include "gui/widgets/configuration/config-widget-value.h"
#include "configuration/configuration-window-data-manager.h"

class ConfigGroupBox;

/**
	&lt;check-box caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigCheckBox : public QCheckBox, public ConfigWidgetValue
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,  ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name = 0);
	virtual ~ConfigCheckBox() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
};

#endif
