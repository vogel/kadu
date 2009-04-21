/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_NOTIFICATION_H
#define ACCOUNT_NOTIFICATION_H

#include "accounts/account.h"
#include "kadu_parser.h"
#include "notification.h"

class Chat;
class ContactSet;

class AccountNotification : public Notification
{
	Q_OBJECT

	Account *CurrentAccount;

public:
	AccountNotification(Account *account, const QString &type, const QIcon &icon, Chat *chat);
	virtual ~AccountNotification();

	Account * account() const { return CurrentAccount; }

};

#endif // ACCOUNT_NOTIFICATION_H
