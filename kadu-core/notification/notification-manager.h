/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtWidgets/QGroupBox>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "status/status.h"

class Action;
class AggregateNotification;
class Group;
class Message;
class MultilogonSession;
class Notification;
class Notifier;

/**
 * @defgroup notify Notify
 * @{
 */

class KADUAPI NotificationManager : public QObject
{
	Q_OBJECT

	QList<Notifier *> Notifiers;
	QStringList IgnoredAccounts;

	QHash<QString, AggregateNotification*> ActiveNotifications;
	QHash<QString, QTimer*> PeriodicNotifications;

	AggregateNotification * findGroup(Notification *notification);

private slots:
	void removeGrouped(Notification *notification);
	void removePeriodicEntries();

public:
	Q_INVOKABLE explicit NotificationManager(QObject *parent = nullptr);
	virtual ~NotificationManager();

	void notify(Notification *notification);

	void registerNotifier(Notifier *notifier);
	void unregisterNotifier(Notifier *notifier);

	const QList<Notifier *> & notifiers() const;

	QString notifyConfigurationKey(const QString &eventType);
	void ignoreConnectionErrors(Account account);

public slots:
	void unignoreConnectionErrors(Account account);

signals:
	void notiferRegistered(Notifier *notifier);
	void notiferUnregistered(Notifier *notifier);

};

/** @} */

#endif // NOTIFICATION_MANAGER_H
