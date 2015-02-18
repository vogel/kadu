/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "sound-exports.h"

#include <injeqt/injeqt.h>
#include <QtCore/QObject>
#include <QtCore/QPointer>

class SoundPlayer;
class SoundThemeManager;

class QSound;

class SOUNDAPI SoundManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SoundManager(QObject *parent = nullptr);
	virtual ~SoundManager();

	void setPlayer(SoundPlayer *player);
	bool isMuted() const;

public slots:
	QObject * playFile(const QString &soundFile, bool force = false, bool stopCurrentlyPlaying = false);
	QObject * playSoundByName(const QString &soundName);
	QObject * testSoundPlaying();
	void stopSound();

	void setMute(bool mute);

private:
	QPointer<SoundThemeManager> m_soundThemeManager;
	QPointer<SoundPlayer> m_player;
	QPointer<QSound> m_playingSound;
	QPointer<QObject> m_soundObject;

	bool m_mute;

	void createDefaultConfiguration();

private slots:
	INJEQT_SETTER void setSoundThemeManager(SoundThemeManager *soundThemeManager);

};
