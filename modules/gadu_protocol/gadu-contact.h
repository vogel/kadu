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

#include "contacts/contact.h"

#include "gadu-protocol.h"

class Account;

class GaduContactAccountData : public Contact
{
	unsigned long MaxImageSize;
	unsigned int GaduProtocolVersion;

public:
	GaduContactAccountData(Account account, Buddy buddy, const QString &id, bool loaded = false) :
			Contact(account, buddy, id, loaded) {}
	GaduContactAccountData(Account account, Buddy buddy, const QString &id, StoragePoint *storage) :
			Contact(account, buddy, id, storage) {}
	GaduContactAccountData(StoragePoint *storage) :
			Contact(storage) {}

	virtual bool validateId();

	GaduProtocol::UinType uin();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

	unsigned int gaduProtocolVersion() { return GaduProtocolVersion; }
	void setGaduProtocolVersion(unsigned int gaduProtocolVersion) { GaduProtocolVersion = gaduProtocolVersion; }

};

#endif // GADU_CONTACT_ACCOUNT_DATA
