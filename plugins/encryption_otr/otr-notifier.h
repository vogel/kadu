/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef OTR_NOTIFIER_H
#define OTR_NOTIFIER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

class Account;
class ChatWidgetRepository;
class Contact;
class NotifyEvent;

class OtrNotifier : public QObject
{
	Q_OBJECT

	static QString OtrNotifyTopic;
	static QString CreatePrivateKeyStartedNotifyTopic;
	static QString CreatePrivateKeyFinishedNotifyTopic;

	QPointer<ChatWidgetRepository> MyChatWidgetRepository;

	QScopedPointer<NotifyEvent> OtrNotifyEvent;
	QScopedPointer<NotifyEvent> CreatePrivateKeyStartedNotifyEvent;
	QScopedPointer<NotifyEvent> CreatePrivateKeyFinishedNotifyEvent;

	void notify(const QString &topic, const Account &account, const QString &message);
	void notify(const Contact &contact, const QString &message);

public:
	explicit OtrNotifier(QObject *parent = 0);
	virtual ~OtrNotifier();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	QList<NotifyEvent *> notifyEvents();

public slots:
	void notifyTryingToStartSession(const Contact &contact);
	void notifyTryingToRefreshSession(const Contact &contact);
	void notifyPeerEndedSession(const Contact &contact);
	void notifyGoneSecure(const Contact &contact);
	void notifyGoneInsecure(const Contact &contact);
	void notifyStillSecure(const Contact &contact);
	void notifyCreatePrivateKeyStarted(const Account &account);
	void notifyCreatePrivateKeyFinished(const Account &account, bool ok);

};

#endif // OTR_NOTIFIER_H
