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

class AutoHide;
class AutoHideConfigurationUiHandler;
class ConfigurationUiHandlerRepository;
class PathsProvider;

class AutoHidePluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutoHidePluginObject(QObject *parent = nullptr);
	virtual ~AutoHidePluginObject();

	virtual void init();
	virtual void done();

private:
	QPointer<AutoHide> m_autoHide;
	QPointer<AutoHideConfigurationUiHandler> m_autoHideConfigurationUiHandler;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_SETTER void setAutoHide(AutoHide *autoHide);
	INJEQT_SETTER void setAutoHideConfigurationUiHandler(AutoHideConfigurationUiHandler *autoHideConfigurationUiHandler);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
