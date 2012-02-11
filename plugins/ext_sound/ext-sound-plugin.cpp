/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "plugins/sound/sound-manager.h"

#include "external-player.h"

#include "ext-sound-plugin.h"

ExtSoundPlugin::~ExtSoundPlugin()
{
}

int ExtSoundPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ExternalPlayer::createInstance();
	SoundManager::instance()->setPlayer(ExternalPlayer::instance());
	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/ext_sound.ui"));

	return 0;
}

void ExtSoundPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/ext_sound.ui"));
	SoundManager::instance()->setPlayer(0);
	ExternalPlayer::destroyInstance();
}

Q_EXPORT_PLUGIN2(ext_sound, ExtSoundPlugin)
