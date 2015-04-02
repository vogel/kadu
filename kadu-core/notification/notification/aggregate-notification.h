/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AGGREGATE_NOTIFICATION_H
#define AGGREGATE_NOTIFICATION_H

#include "notification/notification/notification.h"

class KADUAPI AggregateNotification : public Notification
{
	Q_OBJECT

	QString GroupKey;
	QString Identifier;
	QList<Notification *> Notifications;
	bool IsPeriodic;
	int Period;

public:
	void addNotification(Notification * notification);
	AggregateNotification(Notification *firstNotification);

	QString identifier() { return Identifier; }

	virtual const QString title() const;

	virtual const QString text() const;

	virtual const QStringList details() const;

	virtual const KaduIcon & icon() const { return Notifications.first()->icon(); }

	QList<Notification *> & notifications() { return Notifications; }

	virtual void close();

	const QList<QString> & getCallbacks() { return Notifications.first()->getCallbacks(); }

	void clearCallbacks();

	void addCallback(const QString &name);

	virtual bool isPeriodic() { return IsPeriodic; }
	virtual int period() { return Period; }

protected slots:
	void partialNotificationClosed(Notification *notification);

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();
};

#endif // AGGREGATE_NOTIFICATION_H
