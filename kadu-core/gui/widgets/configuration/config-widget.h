/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
