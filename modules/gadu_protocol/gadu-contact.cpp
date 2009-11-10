/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-contact.h"

GaduContact::GaduContact(Account account, Buddy buddy, const QString &id, bool loaded)
{
	setContactAccount(account);
	setOwnerBuddy(buddy);
	setId(id);

// 	if (data())
// 		data()->setLoaded(loaded);
}

GaduContact::GaduContact(Account account, Buddy buddy, const QString &id, StoragePoint* storage)
{
	setContactAccount(account);
	setOwnerBuddy(buddy);
	setId(id);

// 	if (data())
// 		data()->setStorage(storage);
}

GaduContact::GaduContact(StoragePoint *storage)
{
// 	if (data())
// 		data()->setStorage(storage);
}

unsigned int GaduContact::uin()
{
	return id().toUInt();
}

bool GaduContact::validateId()
{
	bool ok;
	id().toUInt(&ok);
	return ok;
}
