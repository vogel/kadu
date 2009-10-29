/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_CONTACT_FILTER_H
#define ACCOUNT_CONTACT_FILTER_H

#include <QtCore/QMetaType>

#include "accounts/account.h"

#include "abstract-contact-filter.h"

class AccountContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	Account CurrentAccount;
	bool Enabled;

public:
	AccountContactFilter(Account account, QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptContact(Contact contact);

};

Q_DECLARE_METATYPE(AccountContactFilter *)

#endif // ACCOUNT_CONTACT_FILTER_H
