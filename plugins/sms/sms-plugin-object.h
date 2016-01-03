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

#pragma once

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class PathsProvider;
class SmsActions;
class SmsConfigurationUiHandler;
class SmsGatewayManager;
class SmsScriptsManager;

class SmsPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SmsPluginObject(QObject *parent = nullptr);
	virtual ~SmsPluginObject();

	virtual void done() override;

private:
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<SmsActions> m_smsActions;
	QPointer<SmsConfigurationUiHandler> m_smsConfigurationUiHandler;
	QPointer<SmsGatewayManager> m_smsGatewayManager;
	QPointer<SmsScriptsManager> m_smsScriptsManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SETTER void setSmsActions(SmsActions *smsActions);
	INJEQT_SETTER void setSmsConfigurationUiHandler(SmsConfigurationUiHandler *smsConfigurationUiHandler);
	INJEQT_SETTER void setSmsGatewayManager(SmsGatewayManager *smsGatewayManager);
	INJEQT_SETTER void setSmsScriptsManager(SmsScriptsManager *smsScriptsManager);

};
