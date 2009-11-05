/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "client/jabber-client.h"
#include "jabber-avatar-service.h"
#include "jabber-protocol.h"
#include "server/jabber-avatar-fetcher.h"
#include "utils/vcard-factory.h"

void JabberAvatarService::fetchAvatar(Contact *contact)
{

	if (contact->id().isEmpty())
		return;


	JabberAvatarFetcher *avatarFetcher = new JabberAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact *, const QByteArray &)),
			this, SIGNAL(avatarFetched(Contact *, const QByteArray &)));
	avatarFetcher->fetchAvatar();
}

