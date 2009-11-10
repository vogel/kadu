/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONTACT
#define TLEN_CONTACT

#include "contacts/contact.h"

#include "tlen-protocol.h"

class Account;

class TlenContact : public Contact
{
	unsigned long MaxImageSize;

public:
	TlenContact(Account account, Buddy buddy, const QString &id, bool loaded = false)
		: Contact(account, buddy, id, loaded) {}
	TlenContact(Account account, Buddy buddy, const QString &id, StoragePoint *storage)
		: Contact(account, buddy, id, storage) {}
	TlenContact(StoragePoint *storage) :
		Contact(storage) {}

	virtual bool validateId();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

};

#endif // TLEN_CONTACT
