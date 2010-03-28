/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "debug.h"

#include "alsa-player.h"

extern "C" int alsa_sound_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	alsa_player_slots = new AlsaPlayer;
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));

	sound_manager->setPlayer(alsa_player_slots);

	kdebugf2();
	return 0;
}

extern "C" void alsa_sound_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/alsa_sound.ui"));
	delete alsa_player_slots;
	alsa_player_slots = 0;

	sound_manager->setPlayer(0);

	kdebugf2();
}
