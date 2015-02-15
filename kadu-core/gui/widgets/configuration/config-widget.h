/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_WIDGET_H
#define CONFIG_WIDGET_H

#include "configuration/configuration-window-data-manager.h"

class QDomElement;
class ConfigGroupBox;

/**
	&lt;widget id="id" tool-tip="toolTip" /&gt;

	@arg id - id dostępne dla aplikacji/modułów
	@arg toolTip - podpowiedź dla użytkownika
 **/
class KADUAPI ConfigWidget
{
	friend class ConfigurationWidget;

protected:
	ConfigGroupBox *parentConfigGroupBox;
	QString widgetCaption;
	QString CurrentWidgetId;
	QString ParentWidgetId;
	QString StateDependency;
	QString toolTip;
	ConfigurationWindowDataManager *dataManager;

	virtual void createWidgets() = 0;

public:
	ConfigWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigWidget() {}

	virtual void loadConfiguration() = 0;
	virtual void saveConfiguration() = 0;

	virtual bool fromDomElement(QDomElement domElement);

	QString parentWidgetId() { return ParentWidgetId; }
	QString currentWidgetId() { return CurrentWidgetId; }
	bool isStateDependentDirectly() { return StateDependency == "" || StateDependency == "direct"; }
};

#endif
