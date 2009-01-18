/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNTS_AWARE_OBJECT
#define ACCOUNTS_AWARE_OBJECT

#include <QtCore/QList>

#include "exports.h"

class Account;

class KADUAPI AccountsAwareObject 
{
	static QList<AccountsAwareObject *> Objects;

	static void registerObject(AccountsAwareObject *object);
	static void unregisterObject(AccountsAwareObject *object);

protected:
	virtual void accountRegistered(Account *account) = 0;
	virtual void accountUnregistered(Account *account) = 0;

public:
	static void notifyAccountRegistered(Account *account);
	static void notifyAccountUnregistered(Account *account);

	AccountsAwareObject();
	virtual ~AccountsAwareObject();

	void triggerAllAccountsRegistered();
	void triggerAllAccountsUnregistered();

};

#endif // ACCOUNTS_AWARE_OBJECT
