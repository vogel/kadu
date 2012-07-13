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

#include <QtCore/QString>
#include <QtCore/QTimer>

#include "connection-error-notification-manager.h"

ConnectionErrorNotificationManager *ConnectionErrorNotificationManager::Instance = 0;

ConnectionErrorNotificationManager * ConnectionErrorNotificationManager::instance()
{
	if (!Instance)
		Instance = new ConnectionErrorNotificationManager();

	return Instance;
}

void ConnectionErrorNotificationManager::addActiveError(const Account &account, const QString &errorMessage)
{
	ActiveErrors[account].append(errorMessage);
}

void ConnectionErrorNotificationManager::removeActiveError(int delay, const Account &account, const QString &errorMessage)
{
	ActiveErrorsToRemove.enqueue(qMakePair(account, errorMessage));
	QTimer::singleShot(delay, this, SLOT(removeActiveErrorFromQueue()));
}

bool ConnectionErrorNotificationManager::hasActiveError(const Account &account, const QString &errorMessage) const
{
	return ActiveErrors.value(account).contains(errorMessage);
}

void ConnectionErrorNotificationManager::removeActiveErrorFromQueue()
{
	if (ActiveErrorsToRemove.isEmpty())
		return;

	QPair<Account, QString> pair = ActiveErrorsToRemove.dequeue();
	Account &account = pair.first;
	QString &errorMessage = pair.second;

	QStringList &list = ActiveErrors[account];
	list.removeOne(errorMessage);
	if (list.isEmpty())
		ActiveErrors.remove(account);
}
