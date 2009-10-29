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

#include "contacts/account-data/contact-account-data.h"

#include "gadu-protocol.h"

class Account;

class GaduContactAccountData : public ContactAccountData
{
	unsigned long MaxImageSize;
	unsigned int GaduProtocolVersion;

public:
	GaduContactAccountData(Account account, Contact contact, const QString &id, bool loaded = false) :
			ContactAccountData(account, contact, id, loaded) {}
	GaduContactAccountData(Account account, Contact contact, const QString &id, StoragePoint *storage) :
			ContactAccountData(account, contact, id, storage) {}
	GaduContactAccountData(StoragePoint *storage) :
			ContactAccountData(storage) {}

	virtual bool validateId();

	GaduProtocol::UinType uin();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

	unsigned int gaduProtocolVersion() { return GaduProtocolVersion; }
	void setGaduProtocolVersion(unsigned int gaduProtocolVersion) { GaduProtocolVersion = gaduProtocolVersion; }

};

#endif // GADU_CONTACT_ACCOUNT_DATA
