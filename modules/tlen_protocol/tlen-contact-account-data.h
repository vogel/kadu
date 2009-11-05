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

#include "contacts/contact.h"

#include "tlen-protocol.h"

class Account;

class TlenContact : public Contact
{
	unsigned long MaxImageSize;

public:
	TlenContact(Account *account, Contact contact, const QString &id, bool loaded = false)
		: Contact(account, contact, id, loaded) {}
	TlenContact(Account *account, Contact contact, const QString &id, StoragePoint *storage)
		: Contact(account, contact, id, storage) {}
	TlenContact(StoragePoint *storage) :
		Contact(storage) {}

	virtual bool validateId();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

};

#endif // JABBER_CONTACT_ACCOUNT_DATA
