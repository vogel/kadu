/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_WIDGET_VALUE_H
#define CONFIG_WIDGET_VALUE_H

#include "gui/widgets/configuration/config-widget.h"
#include "configuration/configuration-window-data-manager.h"

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