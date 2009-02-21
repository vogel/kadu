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

#include "gadu.h"

class Account;

class GaduContactAccountData : public ContactAccountData
{
	unsigned long MaxImageSize;
	unsigned int GaduProtocolVersion;

public:
	GaduContactAccountData(Contact contact, Account *account, const QString &id = QString::null)
			: ContactAccountData(contact, account, id), GaduProtocolVersion(0) {}

	virtual bool validateId();

	UinType uin();

	void setMaxImageSize(unsigned long maxImageSize) { MaxImageSize = maxImageSize; }

	int gaduProtocolVersion() { return GaduProtocolVersion; }
	void setGaduProtocolVersion(int gaduProtocolVersion) { GaduProtocolVersion = gaduProtocolVersion; }

};

#endif // GADU_CONTACT_ACCOUNT_DATA
