/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QTextDocument>

#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "mediaplayer-notification.h"

NotifyEvent * MediaPlayerNotification::TitleHintNotification = 0;

void MediaPlayerNotification::registerNotifications()
{
	if (!TitleHintNotification)
	{
		TitleHintNotification = new NotifyEvent("MediaPlayerOsd", NotifyEvent::CallbackNotRequired, QT_TRANSLATE_NOOP("@default", "Pseudo-OSD for MediaPlayer"));
		NotificationManager::instance()->registerNotifyEvent(TitleHintNotification);
	}
}

void MediaPlayerNotification::unregisterNotifications()
{
	if (TitleHintNotification)
	{
		NotificationManager::instance()->unregisterNotifyEvent(TitleHintNotification);
		delete TitleHintNotification;
		TitleHintNotification = 0;
	}
}

void MediaPlayerNotification::notifyTitleHint(const QString &title)
{
	Notification *notification = new MediaPlayerNotification();
	notification->setText(Qt::escape(title));
	NotificationManager::instance()->notify(notification);
}

MediaPlayerNotification::MediaPlayerNotification() :
		Notification("MediaPlayerOsd", KaduIcon("external_modules/mediaplayer-media-playback-play"))
{
}

MediaPlayerNotification::~MediaPlayerNotification()
{

}

#include "moc_mediaplayer-notification.cpp"
