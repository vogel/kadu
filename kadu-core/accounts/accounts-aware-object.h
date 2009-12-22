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

#include "aware-object.h"

class Account;

class KADUAPI AccountsAwareObject : public AwareObject<AccountsAwareObject>
{

protected:
	virtual void accountAdded(Account account);
	virtual void accountRemoved(Account account);
	virtual void accountRegistered(Account account) = 0;
	virtual void accountUnregistered(Account account) = 0;

public:
	static void notifyAccountAdded(Account account);
	static void notifyAccountRemoved(Account account);
	static void notifyAccountRegistered(Account account);
	static void notifyAccountUnregistered(Account account);
	
	void triggerAllAccountsAdded();
	void triggerAllAccountsRemoved();
	void triggerAllAccountsRegistered();
	void triggerAllAccountsUnregistered();

};

#endif // ACCOUNTS_AWARE_OBJECT
