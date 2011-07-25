/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef ACCOUNT_NOTIFICATION_H
#define ACCOUNT_NOTIFICATION_H

#include "accounts/account.h"
#include "notification.h"
#include "exports.h"

class BuddySet;

class KADUAPI AccountNotification : public Notification
{
	Q_OBJECT

	Account CurrentAccount;

public:
	static void registerParserTags();
	static void unregisterParserTags();

	AccountNotification(Account account, const QString &type, const KaduIcon &icon);
	virtual ~AccountNotification();

	Account account() const { return CurrentAccount; }

};

#endif // ACCOUNT_NOTIFICATION_H
