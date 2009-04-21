/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "jid-util.h"
#include "jabber-contact-account-data.h"

bool JabberContactAccountData::validateId()
{
	XMPP::Jid newJid(JIDUtil::accountFromString(id()));
	if (newJid.node().isEmpty() || newJid.domain().isEmpty())
		return false;
	return true;
}
