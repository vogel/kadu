/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "plugins/sound/sound-manager.h"

#include "phonon-player.h"

#include "phonon-plugin.h"

PhononPlugin::~PhononPlugin()
{
}

bool PhononPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	PhononPlayer::createInstance();
	SoundManager::instance()->setPlayer(PhononPlayer::instance());

	return true;
}

void PhononPlugin::done()
{
	SoundManager::instance()->setPlayer(0);
	PhononPlayer::destroyInstance();
}

Q_EXPORT_PLUGIN2(phonon_sound, PhononPlugin)

#include "moc_phonon-plugin.cpp"
