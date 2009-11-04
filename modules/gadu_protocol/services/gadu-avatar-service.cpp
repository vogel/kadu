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

#include "gadu-avatar-service.h"
#include "misc/path-conversion.h"
#include "server/gadu-avatar-fetcher.h"

void GaduAvatarService::fetchAvatar(Contact *contactAccountData)
{
	if (contactAccountData->id().isEmpty())
		return;

	GaduAvatarFetcher *avatarFetcher = new GaduAvatarFetcher(contactAccountData, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact *, const QByteArray &)),
			this, SIGNAL(avatarFetched(Contact *, const QByteArray &)));
	avatarFetcher->fetchAvatar();
}
