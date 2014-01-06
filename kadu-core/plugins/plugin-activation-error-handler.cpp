/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugin-activation-error-handler.h"

#include "gui/windows/plugin-error-dialog.h"
#include "plugins/plugin-activation-service.h"
#include "plugins/plugin-state.h"
#include "plugins/plugin-state-service.h"

#include <QtCore/QTimer>

PluginActivationErrorHandler::PluginActivationErrorHandler(QObject *parent) :
		QObject{parent}
{
}

PluginActivationErrorHandler::~PluginActivationErrorHandler()
{
}

void PluginActivationErrorHandler::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
}

void PluginActivationErrorHandler::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Shows activation error to the user.
 * @param errorMessage error message that will be displayer to the user
 * @param activationReason plugin activation reason
 * @todo it really shouldn't call gui classes directly
 *
 * This method creates new PluginErrorDialog with message \p errorMessage and opens it. Depending on
 * \p activationReason, it also intructs the dialog wheter to offer the user choice wheter to try
 * to load this plugin automatically in future.
 */
void PluginActivationErrorHandler::handleActivationError(const QString &pluginName, const QString &errorMessage)
{
	if (pluginName.isEmpty())
		return;

	auto state = m_pluginStateService ? m_pluginStateService.data()->pluginState(pluginName) : PluginState::Disabled;
	auto offerLoadInFutureChoice = PluginState::Enabled == state;

	// TODO: set parent to MainConfigurationWindow is it exists
	auto errorDialog = new PluginErrorDialog{pluginName, errorMessage, offerLoadInFutureChoice, 0};
	if (offerLoadInFutureChoice)
		connect(errorDialog, SIGNAL(accepted(QString,bool)), this, SLOT(setStateEnabledIfInactive(QString,bool)));

	QTimer::singleShot(0, errorDialog, SLOT(open()));
}

/**
 * @author Bartosz 'beevvy' Brachaczek
 * @short Sets state enablement of plugin if it is inactive.
 *
 * If this plugin is active or its state is PluginState::New, this method does nothing.
 *
 * Otherwise, this method sets its state to PluginState::Enabled if \p enable is true.
 * If \p enable is false, this method sets the plugin's state to PluginState::Disabled.
 */
void PluginActivationErrorHandler::setStateEnabledIfInactive(const QString &pluginName, bool enable)
{
	if (!m_pluginActivationService || !m_pluginStateService)
		return;

	if (m_pluginActivationService.data()->isActive(pluginName))
		return;

	m_pluginStateService.data()->setPluginState(pluginName, enable ? PluginState::Enabled : PluginState::Disabled);
}

#include "moc_plugin-activation-error-handler.cpp"
