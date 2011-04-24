/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_WIZARD_CONFIGURATION_UI_HANDLER
#define CONFIG_WIZARD_CONFIGURATION_UI_HANDLER

#include <QtCore/QObject>

class QAction;

class ActionDescription;

class ConfigWizardConfigurationUiHandler : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ConfigWizardConfigurationUiHandler)

	static ConfigWizardConfigurationUiHandler *Instance;

	ActionDescription *ShowConfigWizardActionDescription;

	ConfigWizardConfigurationUiHandler();
	virtual ~ConfigWizardConfigurationUiHandler();
	
public slots:
	void showConfigWizardSlot();

public:
	static void registerActions(bool firstLoad);
	static void unregisterActions();

	static ConfigWizardConfigurationUiHandler * instance();

	void showConfigWizard();

};

#endif // CONFIG_WIZARD_CONFIGURATION_UI_HANDLER
