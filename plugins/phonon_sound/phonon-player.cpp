/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "phonon-player.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <phonon/mediaobject.h>
#include <phonon/phononnamespace.h>

PhononPlayer::PhononPlayer(QObject *parent) :
		SoundPlayer{parent}
{
}

PhononPlayer::~PhononPlayer()
{
	if (m_phononPlayer)
		m_phononPlayer->deleteLater();
}

QObject * PhononPlayer::playSound(const QString &path)
{
	if (m_phononPlayer)
		return nullptr;

	auto fileInfo = QFileInfo{path};
	if (!fileInfo.exists())
		return nullptr;

	m_phononPlayer = Phonon::createPlayer(Phonon::NotificationCategory);
	connect(m_phononPlayer, SIGNAL(finished()), m_phononPlayer, SLOT(deleteLater()));
	m_phononPlayer->setCurrentSource(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
	m_phononPlayer->play();
	return m_phononPlayer;
}

#include "moc_phonon-player.cpp"
