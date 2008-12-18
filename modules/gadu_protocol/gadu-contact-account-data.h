/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_ACCOUNT_DATA
#define GADU_CONTACT_ACCOUNT_DATA

#include "contacts/contact-account-data.h"

class Account;

class GaduContactAccountData : public ContactAccountData
{

public:
	GaduContactAccountData(Account *account, const QString &id)
		: ContactAccountData(account, id)
	{}

};

#endif // GADU_CONTACT_ACCOUNT_DATA
