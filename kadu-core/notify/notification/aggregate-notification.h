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

// deleted:
// TODO: we should probably define a simple read-only interface for notification objects and inherit only it in this class
	void addCallback(const QString &caption, const char *slot, const char *signature);
	void setDefaultCallback(int timeout, const char *slot);
	void setTitle(const QString &title);
	void setText(const QString &text);
	void setDetails(const QStringList &details);
	void setIcon(const KaduIcon &icon);

public:
	explicit AggregateNotification(Notification *firstNotification);
	virtual ~AggregateNotification();

	void addNotification(Notification *notification);
	QList<Notification *> & notifications() { return Notifications; }

	virtual void close();
	virtual bool requireCallback() { return Notifications.first()->requireCallback(); }
	// type?
	// key?
	// groupkey?
	virtual QString identifier() { return Notifications.first()->identifier(); }
	virtual const QString title() const;
	virtual const QString text() const;
	virtual const QStringList details() const;
	virtual bool isPeriodic() { return Notifications.first()->isPeriodic(); }
	virtual int period() { return Notifications.first()->period(); }
	virtual const KaduIcon & icon() const { return Notifications.first()->icon(); }
	virtual const QList<Callback> & getCallbacks() { return Notifications.first()->getCallbacks(); }
	virtual QObject * callbackObject() { return Notifications.first(); }

public slots:
	virtual void callbackAccept() { Notifications.first()->callbackAccept(); }
	virtual void callbackDiscard() { Notifications.first()->callbackDiscard(); }
	virtual void clearDefaultCallback() { Notifications.first()->clearDefaultCallback(); }

};

#endif // AGGREGATE_NOTIFICATION_H
