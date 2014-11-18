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

#include "exports.h"

#include <QtCore/QObject>

class Account;
class NotifyEvent;

/**
 * @addtogroup Roster
 * @{
 */

/**
 * @enum RosterNotifier
 * @short Notifier for roster-based events.
 *
 * Provides five user events:
 * * rosterNotifyEvent;
 * * importSucceededNotifyEvent;
 * * importFailedNotifyEvent;
 * * exportSucceededNotifyEvent;
 * * exportFailedNotifyEvent;
 *
 * First one is used as a base for rest.
 */
class KADUAPI RosterNotifier : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit RosterNotifier(QObject *parent = nullptr);
	virtual ~RosterNotifier();

	QList<NotifyEvent *> notifyEvents();

public slots:
	void notifyImportSucceeded(const Account &account);
	void notifyImportFailed(const Account &account);
	void notifyExportSucceeded(const Account &account);
	void notifyExportFailed(const Account &account);

private:
	static QString sm_rosterNotifyTopic;
	static QString sm_importSucceededNotifyTopic;
	static QString sm_importFailedNotifyTopic;
	static QString sm_exportSucceededNotifyTopic;
	static QString sm_exportFailedNotifyTopic;

	QScopedPointer<NotifyEvent> m_rosterNotifyEvent;
	QScopedPointer<NotifyEvent> m_importSucceededNotifyEvent;
	QScopedPointer<NotifyEvent> m_importFailedNotifyEvent;
	QScopedPointer<NotifyEvent> m_exportSucceededNotifyEvent;
	QScopedPointer<NotifyEvent> m_exportFailedNotifyEvent;

	void notify(const QString &topic, const Account &account, const QString &message);

};

/**
 * @}
 */
