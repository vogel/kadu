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

#include "speech-plugin-object.h"

#include "speech.h"
#include "speech-configuration-ui-handler.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "notification/notifier-repository.h"

SpeechPluginObject::SpeechPluginObject(QObject *parent) :
		QObject{parent}
{
}

SpeechPluginObject::~SpeechPluginObject()
{
}

void SpeechPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void SpeechPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void SpeechPluginObject::setNotifierRepository(NotifierRepository *notifierRepository)
{
	m_notifierRepository = notifierRepository;
}

void SpeechPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SpeechPluginObject::setSpeechConfigurationUiHandler(SpeechConfigurationUiHandler *speechConfigurationUiHandler)
{
	m_speechConfigurationUiHandler = speechConfigurationUiHandler;
}

void SpeechPluginObject::setSpeech(Speech *speech)
{
	m_speech = speech;
}

void SpeechPluginObject::init()
{

	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/speech.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_speechConfigurationUiHandler);
	m_notifierRepository->registerNotifier(m_speech);
}

void SpeechPluginObject::done()
{
	m_notifierRepository->unregisterNotifier(m_speech);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_speechConfigurationUiHandler);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/speech.ui"));
}

#include "moc_speech-plugin-object.cpp"
