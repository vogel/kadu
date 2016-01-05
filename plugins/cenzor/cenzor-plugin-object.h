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

class CenzorConfigurationUiHandler;
class CenzorMessageFilter;
class ConfigurationUiHandlerRepository;
class MessageFilterService;
class NotificationEventRepository;
class PathsProvider;

class CenzorPluginObject : public PluginObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit CenzorPluginObject(QObject *parent = nullptr);
	virtual ~CenzorPluginObject();

private:
	QPointer<CenzorConfigurationUiHandler> m_cenzorConfigurationUiHandler;
	QPointer<CenzorMessageFilter> m_cenzorMessageFilter;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MessageFilterService> m_messageFilterService;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setCenzorConfigurationUiHandler(CenzorConfigurationUiHandler *cenzorConfigurationUiHandler);
	INJEQT_SET void setCenzorMessageFilter(CenzorMessageFilter *cenzorMessageFilter);
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setMessageFilterService(MessageFilterService *messageFilterService);
	INJEQT_SET void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

};
