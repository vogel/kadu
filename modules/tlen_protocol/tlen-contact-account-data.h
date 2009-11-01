/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONTACT_ACCOUNT_DATA
#define TLEN_CONTACT_ACCOUNT_DATA

#include "buddies/account-data/contact-account-data.h"

#include "tlen-protocol.h"

class Account;

class TlenContactAccountData : public ContactAccountData
{
	unsigned long MaxImageSize;

public:
	TlenContactAccountData(Buddy contact, Account *account, const QString &id = QString::null, bool loadFromConfiguration = true)
		: ContactAccountData(contact, account, id, loadFromConfiguration)
	{}

	virtual bool validateId();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

};

#endif // JABBER_CONTACT_ACCOUNT_DATA
