/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtCore/QDebug>
#include <QtXml/QDomDocument>

#include "tlen-avatar-service.h"
#include "misc/path-conversion.h"
#include "server/tlen-avatar-fetcher.h"

void TlenAvatarService::fetchAvatar(ContactAccountData *contactAccountData)
{
	if (contactAccountData->id().isEmpty())
		return;

	if (inProgress.contains(contactAccountData))
		return;

	TlenAvatarFetcher *avatarFetcher = new TlenAvatarFetcher(contactAccountData, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(ContactAccountData *, QPixmap)),
			this, SIGNAL(avatarReady(ContactAccountData *, QPixmap)));

	avatarFetcher->fetchAvatar();
}

void TlenAvatarService::avatarReady(ContactAccountData *contactAccountData, QPixmap avatar)
{
	inProgress.removeAll(contactAccountData);
	emit avatarFetched(contactAccountData, avatar);
}
