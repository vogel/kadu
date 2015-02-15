/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_WIDGET_VALUE_H
#define CONFIG_WIDGET_VALUE_H

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-widget.h"

class QDomElement;
class ConfigGroupBox;
class ConfigurationWindowDataManager;

/**
	&lt;widget config-section="section" config-item="item" /&gt;

	@arg section - sekcja w której trzymany jest element
	@arg item - element, w którym trzymana jest wartość parametry
 **/
class KADUAPI ConfigWidgetValue : public ConfigWidget
{
protected:
	QString section;
	QString item;

public:
	ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigWidgetValue() {}

	virtual bool fromDomElement(QDomElement domElement);
};

#endif
