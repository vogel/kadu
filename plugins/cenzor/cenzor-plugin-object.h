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

public:
	Q_INVOKABLE explicit CenzorPluginObject(QObject *parent = nullptr);
	virtual ~CenzorPluginObject();

	virtual void init();
	virtual void done();

private:
	QPointer<CenzorConfigurationUiHandler> m_cenzorConfigurationUiHandler;
	QPointer<CenzorMessageFilter> m_cenzorMessageFilter;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MessageFilterService> m_messageFilterService;
	QPointer<NotificationEventRepository> m_notificationEventRepository;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_SETTER void setCenzorConfigurationUiHandler(CenzorConfigurationUiHandler *cenzorConfigurationUiHandler);
	INJEQT_SETTER void setCenzorMessageFilter(CenzorMessageFilter *cenzorMessageFilter);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setMessageFilterService(MessageFilterService *messageFilterService);
	INJEQT_SETTER void setNotificationEventRepository(NotificationEventRepository *notificationEventRepository);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
