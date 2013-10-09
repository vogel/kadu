/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef MULTILOGON_NOTIFICATION_H
#define MULTILOGON_NOTIFICATION_H

#include "notify/notification-manager.h"
#include "notify/notification/account-notification.h"

class MultilogonSession;
class NotifyEvent;

class MultilogonNotification : public AccountNotification
{
	Q_OBJECT

	static NotifyEvent *MultilogonSessionNotifyEvent;
	static NotifyEvent *MultilogonSessionConnectedNotifyEvent;
	static NotifyEvent *MultilogonSessionDisconnectedNotifyEvent;

	MultilogonSession *Session;

private slots:
	void killSession();

public:
	MultilogonNotification(MultilogonSession *session, const QString &type, bool addKillCallback);
	virtual ~MultilogonNotification();

	static void registerEvents();
	static void unregisterEvents();
};

#endif // MULTILOGON_NOTIFICATION_H
