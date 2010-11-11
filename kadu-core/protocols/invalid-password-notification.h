/*
 * %kadu copyright begin%
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef INVALID_PASSWORD_NOTIFICATION_H
#define INVALID_PASSWORD_NOTIFICATION_H

#include "notify/account-notification.h"

class NotifyEvent;

class InvalidPasswordNotification : public AccountNotification
{
	Q_OBJECT

	static NotifyEvent *InvalidPasswordNotifyEvent;
	static QSet<Account> ActiveErrors;

public:
	static void registerEvent();
	static void unregisterEvent();

	static bool activeError(Account account);

	InvalidPasswordNotification(Account account);
	virtual ~InvalidPasswordNotification();

	virtual bool requireCallback();
	virtual bool ignoreSilentMode();

public slots:
	virtual void callbackAccept();

};

#endif // INVALID_PASSWORD_NOTIFICATION_H
