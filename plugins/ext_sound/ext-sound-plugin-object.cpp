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

#include "ext-sound-plugin-object.h"

#include "external-player.h"

#include "plugins/sound/sound-manager.h"
#include "plugins/sound/sound-plugin-object.h"

#include "gui/windows/main-configuration-window.h"
#include "kadu/kadu-core/misc/paths-provider.h"
#include "plugin/plugin-repository.h"

ExtSoundPluginObject::ExtSoundPluginObject(QObject *parent) :
		PluginObject{parent}
{
}

ExtSoundPluginObject::~ExtSoundPluginObject()
{
}

void ExtSoundPluginObject::setExternalPlayer(ExternalPlayer *externalPlayer)
{
	m_externalPlayer = externalPlayer;
}

void ExtSoundPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void ExtSoundPluginObject::setPluginRepository(PluginRepository *pluginRepository)
{
	m_pluginRepository = pluginRepository;
}

void ExtSoundPluginObject::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

void ExtSoundPluginObject::init()
{
	m_soundManager->setPlayer(m_externalPlayer);
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String{"plugins/configuration/ext_sound.ui"});
}

void ExtSoundPluginObject::done()
{
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String{"plugins/configuration/ext_sound.ui"});
	m_soundManager->setPlayer(nullptr);
}

#include "moc_ext-sound-plugin-object.cpp"
