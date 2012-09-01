/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#include "notify/notification/notification.h"

class KADUAPI AggregateNotification : public Notification
{
	Q_OBJECT

	QString GroupKey;
	QList<Notification *> Notifications;

public:
	void addNotification(Notification * notification);
	AggregateNotification(Notification *firstNotification);

	QString identifier() { return Notifications.first()->identifier(); }

	virtual const QString title() const;

	virtual const QString text() const;

	virtual const QStringList details() const;

	virtual const KaduIcon & icon() const { return Notifications.first()->icon(); }

	QList<Notification *> & notifications() { return Notifications; }

	virtual void close();

	const QList<Callback> & getCallbacks() { return Notifications.first()->getCallbacks(); }

	void clearCallbacks();

	void addCallback(const QString &caption, const char *slot, const char *signature);

	void setDefaultCallback(int timeout, const char *slot);

	virtual bool requireCallback();

	virtual bool isPeriodic() { return Notifications.first()->isPeriodic(); }
	virtual int period() { return Notifications.first()->period(); }

protected slots:
	void partialNotificationClosed(Notification *notification);

public slots:
	virtual void callbackAccept();
	virtual void callbackDiscard();
	virtual void clearDefaultCallback();
};

#endif // AGGREGATE_NOTIFICATION_H
