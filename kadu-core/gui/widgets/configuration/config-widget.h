/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/configuration-window.h"

class QDomElement;
class ConfigGroupBox;

/**
	&lt;widget id="id" tool-tip="toolTip" /&gt;

	@arg id - id dostępne dla aplikacji/modułów
	@arg toolTip - podpowiedź dla użytkownika
 **/
class KADUAPI ConfigWidget
{
	friend class ConfigurationWindow;

protected:
	ConfigGroupBox *parentConfigGroupBox;
	QString widgetCaption;
	QString toolTip;
	ConfigurationWindowDataManager *dataManager;

	virtual void createWidgets() = 0;

public:
	ConfigWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigWidget() {}

	virtual void loadConfiguration() = 0;
	virtual void saveConfiguration() = 0;

	virtual void show() = 0;
	virtual void hide() = 0;
	virtual bool fromDomElement(QDomElement domElement);
};

#endif