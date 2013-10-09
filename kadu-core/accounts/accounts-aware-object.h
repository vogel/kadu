/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

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
