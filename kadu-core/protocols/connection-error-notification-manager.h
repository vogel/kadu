/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONNECTION_ERROR_NOTIFICATION_MANAGER_H
#define CONNECTION_ERROR_NOTIFICATION_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QQueue>

class QString;
class QStringList;

class Account;

class ConnectionErrorNotificationManager : public QObject
{
	Q_OBJECT

	static ConnectionErrorNotificationManager *Instance;

	QMap<Account, QStringList> ActiveErrors;
	QQueue<QPair<Account, QString> > ActiveErrorsToRemove;

private slots:
	void removeActiveErrorFromQueue();

public:
	static ConnectionErrorNotificationManager * instance();

	void addActiveError(const Account &account, const QString &errorMessage);
	void removeActiveError(int delay, const Account &account, const QString &errorMessage);

	bool hasActiveError(const Account &account, const QString &errorMessage) const;

};

// for MOC
#include <QtCore/QStringList>
#include "accounts/account.h"

#endif // CONNECTION_ERROR_NOTIFICATION_MANAGER_H
