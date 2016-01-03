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

class Autoaway;
class AutoawayConfigurationUiHandler;
class AutoawayStatusChanger;
class ConfigurationUiHandlerRepository;
class PathsProvider;
class StatusChangerManager;

class AutoawayPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutoawayPluginObject(QObject *parent = nullptr);
	virtual ~AutoawayPluginObject();

private:
	QPointer<Autoaway> m_autoaway;
	QPointer<AutoawayConfigurationUiHandler> m_autoawayConfigurationUiHandler;
	QPointer<AutoawayStatusChanger> m_autoawayStatusChanger;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<StatusChangerManager> m_statusChangerManager;

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SETTER void setAutoaway(Autoaway *autoaway);
	INJEQT_SETTER void setAutoawayConfigurationUiHandler(AutoawayConfigurationUiHandler *autoawayConfigurationUiHandler);
	INJEQT_SETTER void setAutoawayStatusChanger(AutoawayStatusChanger *autoawayStatusChanger);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SETTER void setStatusChangerManager(StatusChangerManager *statusChangerManager);

};
