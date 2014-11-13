/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>

#include <exports.h>

class Account;
class NotifyEvent;

class KADUAPI RosterNotifier : public QObject
{
	Q_OBJECT

	static QString RosterNotifyTopic;
	static QString ImportSucceededNotifyTopic;
	static QString ImportFailedNotifyTopic;
	static QString ExportSucceededNotifyTopic;
	static QString ExportFailedNotifyTopic;

	QScopedPointer<NotifyEvent> RosterNotifyEvent;
	QScopedPointer<NotifyEvent> ImportSucceededNotifyEvent;
	QScopedPointer<NotifyEvent> ImportFailedNotifyEvent;
	QScopedPointer<NotifyEvent> ExportSucceededNotifyEvent;
	QScopedPointer<NotifyEvent> ExportFailedNotifyEvent;

	void notify(const QString &topic, const Account &account, const QString &message);

public:
	explicit RosterNotifier(QObject *parent = 0);
	virtual ~RosterNotifier();

	QList<NotifyEvent *> notifyEvents();

public slots:
	void notifyImportSucceeded(const Account &account);
	void notifyImportFailed(const Account &account);
	void notifyExportSucceeded(const Account &account);
	void notifyExportFailed(const Account &account);

};
