/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "show-config-wizard-action.h"

#include "gui/windows/config-wizard-window.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/actions/actions.h"
#include "gui/actions/action.h"
#include "plugin/plugin-injected-factory.h"
#include "activate.h"

ShowConfigWizardAction::ShowConfigWizardAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setName(QStringLiteral("showConfigWizard"));
	setText(tr("Start Configuration Wizard"));
	setType(ActionDescription::TypeMainMenu);
}

ShowConfigWizardAction::~ShowConfigWizardAction()
{
	delete m_wizard.data();
}

void ShowConfigWizardAction::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ShowConfigWizardAction::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void ShowConfigWizardAction::init()
{
	registerAction(actionsRegistry());
}

void ShowConfigWizardAction::showConfigWindow()
{
	if (m_wizard)
		_activateWindow(m_configuration, m_wizard.data());
	else
	{
		m_wizard = m_pluginInjectedFactory->makeInjected<ConfigWizardWindow>();
		// we have to delay it a bit to show after main window to have focus on startup
		QMetaObject::invokeMethod(m_wizard.data(), "show", Qt::QueuedConnection);
	}
}

void ShowConfigWizardAction::actionTriggered(QAction *, bool)
{
	showConfigWindow();
}

#include "moc_show-config-wizard-action.cpp"
