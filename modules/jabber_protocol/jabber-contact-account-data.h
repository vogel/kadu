/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONTACT_ACCOUNT_DATA
#define JABBER_CONTACT_ACCOUNT_DATA

#include <xmpp.h>

#include "contacts/contact-account-data.h"

class Account;

class JabberContactAccountData : public ContactAccountData
{
	unsigned long MaxImageSize;

public:
	JabberContactAccountData(Contact contact, Account *account, const QString &id = QString::null, bool isAnonymous = false)
		: ContactAccountData(contact, account, id, isAnonymous)
	{}

	virtual bool validateId();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

};

#endif // JABBER_CONTACT_ACCOUNT_DATA
