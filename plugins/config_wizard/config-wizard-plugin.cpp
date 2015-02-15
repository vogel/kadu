/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "config-wizard-configuration-ui-handler.h"

#include "config-wizard-plugin.h"

ConfigWizardPlugin::~ConfigWizardPlugin()
{

}

bool ConfigWizardPlugin::init(bool firstLoad)
{
	ConfigWizardConfigurationUiHandler::registerActions(firstLoad);
	if (firstLoad)
		ConfigWizardConfigurationUiHandler::instance()->showConfigWizard();

	return true;
}

void ConfigWizardPlugin::done()
{
	ConfigWizardConfigurationUiHandler::unregisterActions();
}

Q_EXPORT_PLUGIN2(config_wizard, ConfigWizardPlugin)

#include "moc_config-wizard-plugin.cpp"
