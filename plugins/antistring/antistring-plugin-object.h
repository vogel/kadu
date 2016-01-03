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

#include "misc/memory.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AntistringConfigurationUiHandler;
class AntistringMessageFilter;
class ConfigurationUiHandlerRepository;
class MessageFilterService;
class NotificationEventRepository;
class PathsProvider;

class AntistringPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AntistringPluginObject(QObject *parent = nullptr);
	virtual ~AntistringPluginObject();

private:
	QPointer<AntistringConfigurationUiHandler> m_antistringConfigurationUiHandler;
	QPointer<AntistringMessageFilter> m_antistringMessageFilter;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MessageFilterService> m_messageFilterService;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SETTER void setAntistringConfigurationUiHandler(AntistringConfigurationUiHandler *antistringConfigurationUiHandler);
	INJEQT_SETTER void setAntistringMessageFilter(AntistringMessageFilter *antistringMessageFilter);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setMessageFilterService(MessageFilterService *messageFilterService);
	INJEQT_SETTER void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
