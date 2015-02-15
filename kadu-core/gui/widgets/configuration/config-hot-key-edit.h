/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_HOT_KEY_EDIT_H
#define CONFIG_HOT_KEY_EDIT_H

#include "configuration/configuration-window-data-manager.h"
#include "gui/hot-key.h"
#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;

class ConfigGroupBox;

/**
	&lt;hot-key-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigHotKeyEdit : public HotKeyEdit, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,  ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigHotKeyEdit();

	virtual void setVisible(bool visible);

	virtual void loadConfiguration();
	virtual void saveConfiguration();

};

#endif
