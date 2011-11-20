/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtXml/QDomElement>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-widget-value.h"

#include "debug.h"

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: ConfigWidget(parentConfigGroupBox, dataManager)
{
}

ConfigWidgetValue::ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox, dataManager), section(section), item(item)
{
}

bool ConfigWidgetValue::fromDomElement(QDomElement domElement)
{
	section = domElement.attribute("config-section");
	item = domElement.attribute("config-item");

	return ConfigWidget::fromDomElement(domElement);
}
