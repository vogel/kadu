/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "misc/path-conversion.h"
#include "debug.h"
#include "exports.h"

#include "modules/sound/sound-manager.h"

#include "alsa-player.h"

extern "C" KADU_EXPORT int alsa_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	AlsaPlayer::createInstance();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));

	SoundManager::instance()->setPlayer(AlsaPlayer::instance());

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void alsa_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));
	AlsaPlayer::destroyInstance();

	SoundManager::instance()->setPlayer(0);

	kdebugf2();
}
