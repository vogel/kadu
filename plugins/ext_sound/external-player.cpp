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

#include "external-player.h"


#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

#include <QtCore/QProcess>

ExternalPlayer::ExternalPlayer(QObject *parent) :
		SoundPlayer{parent}
{
	createDefaultConfiguration();
}

ExternalPlayer::~ExternalPlayer()
{
	if (m_playerProcess)
		m_playerProcess->deleteLater();
}

QObject * ExternalPlayer::playSound(const QString &path)
{
	if (m_playerProcess)
		return nullptr;

	auto playerCommand = Application::instance()->configuration()->deprecatedApi()->readEntry("Sounds", "SoundPlayer");
	if (playerCommand.isEmpty())
		return nullptr;

	auto argumentList = QStringList{};
	argumentList.append(path);

	m_playerProcess = new QProcess{this};
	m_playerProcess->start(playerCommand, argumentList);
	connect(m_playerProcess, SIGNAL(finished(int)), m_playerProcess, SLOT(deleteLater()));
	return m_playerProcess;
}

void ExternalPlayer::createDefaultConfiguration()
{
#ifdef Q_OS_MAC
	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "SoundPlayer", "/Applications/Kadu.app/Contents/MacOS/playsound");
#else
	Application::instance()->configuration()->deprecatedApi()->addVariable("Sounds", "SoundPlayer", "/usr/bin/play");
#endif
}

#include "moc_external-player.cpp"
