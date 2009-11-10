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

bool JabberContact::validateId()
{
	XMPP::Jid newJid(JIDUtil::accountFromString(id()));
	return !newJid.node().isEmpty() && !newJid.domain().isEmpty();
}
