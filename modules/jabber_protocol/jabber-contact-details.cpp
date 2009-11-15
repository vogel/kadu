/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "libiris/include/xmpp.h"

#include "utils/jid-util.h"

#include "jabber-contact-details.h"

JabberContactDetails::JabberContactDetails(StoragePoint *storagePoint, Contact parent) :
		ContactDetails(storagePoint, parent),
		MaxImageSize(0)
{
}

JabberContactDetails::~JabberContactDetails()
{
}

bool JabberContactDetails::validateId()
{
	XMPP::Jid newJid(JIDUtil::accountFromString(contact().id()));
	return !newJid.node().isEmpty() && !newJid.domain().isEmpty();
}

void JabberContactDetails::store()
{
}
