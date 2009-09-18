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

void JabberAvatarService::fetchAvatar(ContactAccountData *contactAccountData)
{

	if (contactAccountData->id().isEmpty())
		return;


	JabberAvatarFetcher *avatarFetcher = new JabberAvatarFetcher(contactAccountData, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(ContactAccountData *, QPixmap)),
			this, SIGNAL(avatarFetched(ContactAccountData *, QPixmap)));
	avatarFetcher->fetchAvatar();
}

