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

#include "word-fix-plugin-object.h"

#include "word-fix-configuration-ui-handler.h"
#include "word-fix.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

WordFixPluginObject::WordFixPluginObject(QObject *parent) :
		QObject{parent}
{
}

WordFixPluginObject::~WordFixPluginObject()
{
}

void WordFixPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void WordFixPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void WordFixPluginObject::setWordFixConfigurationUiHandler(WordFixConfigurationUiHandler *wordFixConfigurationUiHandler)
{
	m_wordFixConfigurationUiHandler = wordFixConfigurationUiHandler;
}

void WordFixPluginObject::setWordFix(WordFix *wordFix)
{
	m_wordFix = wordFix;
}

void WordFixPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/word_fix.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_wordFixConfigurationUiHandler);
}

void WordFixPluginObject::done()
{
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_wordFixConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/word_fix.ui"));
}

#include "moc_word-fix-plugin-object.cpp"
