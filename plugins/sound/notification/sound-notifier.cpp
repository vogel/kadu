/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#include "sound-notifier.h"

#include "gui/sound-configuration-ui-handler.h"
#include "sound-manager.h"

#include "notification/notification/aggregate-notification.h"
#include "notification/notification/notification.h"

#include <QtCore/QFileInfo>

SoundNotifier::SoundNotifier(QObject *parent) :
		Notifier{"Sound", QT_TRANSLATE_NOOP("@default", "Play a sound"), KaduIcon{"audio-volume-high"}, parent}
{
}

SoundNotifier::~SoundNotifier()
{
}

void SoundNotifier::setSoundConfigurationUiHandler(SoundConfigurationUiHandler *soundConfigurationUiHandler)
{
	m_soundConfigurationUiHandler = soundConfigurationUiHandler;
}

void SoundNotifier::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

void SoundNotifier::notify(Notification *notification)
{
	auto aggregate = qobject_cast<AggregateNotification *>(notification);
	if (aggregate)
		notification = aggregate->notifications().front();

	auto chat = notification->data()["chat"].value<Chat>();
	if (chat && chat.property("sound:use_custom_sound", false).toBool())
	{
		// we need abstraction for that
		auto customSound = chat.property("sound:custom_sound", QString{}).toString();
		auto fileInfo = QFileInfo{customSound};
		if (fileInfo.exists())
		{
			m_soundManager->playFile(customSound);
			return;
		}
	}

	if (!chat.contacts().isEmpty())
	{
		auto buddy = chat.contacts().begin()->ownerBuddy();
		if (buddy && buddy.property("sound:use_custom_sound", false).toBool())
		{
			auto customSound = buddy.property("sound:custom_sound", QString{}).toString();
			auto fileInfo = QFileInfo{customSound};
			if (fileInfo.exists())
			{
				m_soundManager->playFile(customSound);
				return;
			}
		}
	}

	m_soundManager->playSoundByName(notification->key());
}

NotifierConfigurationWidget * SoundNotifier::createConfigurationWidget(QWidget* parent)
{
	return m_soundConfigurationUiHandler
			? m_soundConfigurationUiHandler->createConfigurationWidget(parent)
			: nullptr;
}

#include "moc_sound-notifier.cpp"
