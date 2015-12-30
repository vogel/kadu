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

#include "spellchecker-plugin-object.h"

#include "spellchecker-configuration-ui-handler.h"
#include "spellchecker.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

SpellcheckerPluginObject::SpellcheckerPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

SpellcheckerPluginObject::~SpellcheckerPluginObject()
{
}

void SpellcheckerPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void SpellcheckerPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void SpellcheckerPluginObject::setSpellcheckerConfigurationUiHandler(SpellcheckerConfigurationUiHandler *spellcheckerConfigurationUiHandler)
{
	m_spellcheckerConfigurationUiHandler = spellcheckerConfigurationUiHandler;
}

void SpellcheckerPluginObject::setSpellChecker(SpellChecker *spellChecker)
{
	m_spellChecker = spellChecker;
}

void SpellcheckerPluginObject::init()
{
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/spellchecker.ui"));
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_spellcheckerConfigurationUiHandler);
}

void SpellcheckerPluginObject::done()
{
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_spellcheckerConfigurationUiHandler);
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/spellchecker.ui"));
}

#include "moc_spellchecker-plugin-object.cpp"
