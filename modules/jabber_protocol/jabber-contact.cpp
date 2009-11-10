/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "utils/jid-util.h"

#include "jabber-contact.h"

JabberContact::JabberContact(Account account, Buddy buddy, const QString &id, bool loaded)
{
	setContactAccount(account);
	setOwnerBuddy(buddy);
	setId(id);

// 	if (data())
// 		data()->setLoaded(loaded);
}

JabberContact::JabberContact(Account account, Buddy buddy, const QString &id, StoragePoint* storage)
{
	setContactAccount(account);
	setOwnerBuddy(buddy);
	setId(id);

// 	if (data())
// 		data()->setStorage(storage);
}

JabberContact::JabberContact(StoragePoint *storage)
{
// 	if (data())
// 		data()->setStorage(storage);
}

bool JabberContact::validateId()
{
	XMPP::Jid newJid(JIDUtil::accountFromString(id()));
	return !newJid.node().isEmpty() && !newJid.domain().isEmpty();
}
