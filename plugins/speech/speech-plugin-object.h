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

#include "injeqt-type-roles.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class MainConfigurationWindowService;
class NotifierRepository;
class PathsProvider;
class SpeechConfigurationUiHandler;
class Speech;

class SpeechPluginObject : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(PLUGIN)

public:
	Q_INVOKABLE explicit SpeechPluginObject(QObject *parent = nullptr);
	virtual ~SpeechPluginObject();

private:
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MainConfigurationWindowService> m_mainConfigurationWindowService;
	QPointer<NotifierRepository> m_notifierRepository;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<SpeechConfigurationUiHandler> m_speechConfigurationUiHandler;
	QPointer<Speech> m_speech;

private slots:
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService);
	INJEQT_SET void setNotifierRepository(NotifierRepository *notifierRepository);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SET void setSpeechConfigurationUiHandler(SpeechConfigurationUiHandler *speechConfigurationUiHandler);
	INJEQT_SET void setSpeech(Speech *speech);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
