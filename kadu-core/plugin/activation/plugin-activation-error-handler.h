/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class PluginStateService;
class PluginActivationService;

/**
 * @addtogroup Plugin
 * @{
 */

/**
 * @class PluginActivationErrorHandler
 * @short Service for handling plugin activation errors.
 *
 * Service used in PluginManager to handle exceptions of type PluginActivationErrorException.
 *
 * This implementation informs user by displaying error message dialog. In case exception occured
 * during program startup an additional checkbox "Load plugin at next startup" is offered to allow
 * user to enable/disable it. This case occurs when plugin state is already set to PluginState::Enabled
 * (this is checked by using PluginStateService).
 *
 * PluginActivationService is used to check plugin activation state after decision in erorr dialog was
 * made by user (as plugin could be activated in meantime).
 *
 * In other cases checkbox is not displayed (the assumption is that activation can only occur during program
 * startup or from configuration window, where this checkbox is not needed).
 */
class KADUAPI PluginActivationErrorHandler : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(PluginActivationErrorHandler)

public:
	explicit PluginActivationErrorHandler(QObject *parent = nullptr);
	virtual ~PluginActivationErrorHandler();

	void setPluginActivationService(PluginActivationService *pluginActivationService);
	void setPluginStateService(PluginStateService *pluginStateService);

	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @short Shows activation error to the user.
	 * @param pluginName name of plugin that caused error
	 * @param errorMessage error message that will be displayer to the user
	 *
	 * This method creates new PluginErrorDialog with message \p errorMessage and opens it. Depending on
	 * current plugin state, it also intructs the dialog wheter to offer the user choice wheter to try
	 * to load this plugin automatically in future.
	 */
	void handleActivationError(const QString &pluginName, const QString &errorMessage);

private:
	QPointer<PluginActivationService> m_pluginActivationService;
	QPointer<PluginStateService> m_pluginStateService;

private slots:
	/**
	 * @author Bartosz 'beevvy' Brachaczek
	 * @short Sets state enablement of plugin if it is inactive.
	 *
	 * If this plugin is active or its state is PluginState::New, this method does nothing.
	 *
	 * Otherwise, this method sets its state to PluginState::Enabled if \p enable is true.
	 * If \p enable is false, this method sets the plugin's state to PluginState::Disabled.
	 */
	void setStateEnabledIfInactive(const QString &pluginName, bool enable);

};

/**
 * @}
 */
