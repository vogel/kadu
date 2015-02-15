/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "phonon-plugin.h"

#include "phonon-player.h"

#include "plugins/sound/sound-manager.h"
#include "plugins/sound/sound-plugin.h"

PhononPlugin::PhononPlugin(QObject *parent) :
		QObject{parent}
{
}

PhononPlugin::~PhononPlugin()
{
}

bool PhononPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	m_phononPlayer = new PhononPlayer{this};
	SoundPlugin::soundManager()->setPlayer(m_phononPlayer);

	return true;
}

void PhononPlugin::done()
{
	SoundPlugin::soundManager()->setPlayer(nullptr);
	if (m_phononPlayer)
		m_phononPlayer->deleteLater();
}

Q_EXPORT_PLUGIN2(phonon_sound, PhononPlugin)

#include "moc_phonon-plugin.cpp"
