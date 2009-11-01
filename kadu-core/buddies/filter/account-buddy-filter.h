/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_BUDDY_FILTER_H
#define ACCOUNT_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "accounts/account.h"

#include "abstract-buddy-filter.h"

class AccountBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Account CurrentAccount;
	bool Enabled;

public:
	AccountBuddyFilter(Account account, QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy contact);

};

Q_DECLARE_METATYPE(AccountBuddyFilter *)

#endif // ACCOUNT_BUDDY_FILTER_H
