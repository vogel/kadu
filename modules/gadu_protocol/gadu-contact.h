/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_H
#define GADU_CONTACT_H

#include "contacts/contact.h"

#include "gadu-protocol.h"

class Account;

class GaduContact : public Contact
{
	unsigned long MaxImageSize;
	unsigned int GaduProtocolVersion;

public:
	GaduContact(Account account, Buddy buddy, const QString &id, bool loaded = false);
	GaduContact(Account account, Buddy buddy, const QString &id, StoragePoint *storage);
	GaduContact(StoragePoint *storage);

	virtual bool validateId();

	GaduProtocol::UinType uin();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

	unsigned int gaduProtocolVersion() { return GaduProtocolVersion; }
	void setGaduProtocolVersion(unsigned int gaduProtocolVersion) { GaduProtocolVersion = gaduProtocolVersion; }

};

#endif // GADU_CONTACT_H
