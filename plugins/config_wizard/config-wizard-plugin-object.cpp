/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "config-wizard-plugin-object.h"

#include "config-wizard-actions.h"

#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state.h"

ConfigWizardPluginObject::ConfigWizardPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

ConfigWizardPluginObject::~ConfigWizardPluginObject()
{
}

void ConfigWizardPluginObject::setConfigWizardActions(ConfigWizardActions *configWizardActions)
{
	m_configWizardActions = configWizardActions;
}

void ConfigWizardPluginObject::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void ConfigWizardPluginObject::init()
{
	m_configWizardActions->registerActions();

	if (m_pluginStateService->pluginState("config_wizard") == PluginState::New)
		m_configWizardActions->showConfigWizard();
}

void ConfigWizardPluginObject::done()
{
	m_configWizardActions->unregisterActions();
}

#include "moc_config-wizard-plugin-object.cpp"
