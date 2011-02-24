/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_ACTION_BUTTON_H
#define CONFIG_ACTION_BUTTON_H

#include <QtGui/QPushButton>
#include "gui/widgets/configuration/config-widget.h"
#include "configuration/configuration-window-data-manager.h"

class ConfigGroupBox;

/**
	&lt;action-button caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigActionButton : public QPushButton, public ConfigWidget
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigActionButton() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	virtual void show();
	virtual void hide();
};

#endif
