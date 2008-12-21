/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_PREVIEW_H
#define CONFIG_PREVIEW_H

#include "gui/widgets/configuration/config-widget.h"
#include "configuration/configuration-window-data-manager.h"
#include "preview.h"

class QLabel;
class ConfigGroupBox;

/**
	&lt;preview caption="caption" id="id" /&gt;

	Podgląd składni.
 **/
class KADUAPI ConfigPreview : public Preview, public ConfigWidget
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigPreview(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigPreview();

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

#endif
