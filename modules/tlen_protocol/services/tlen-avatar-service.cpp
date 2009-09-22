/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <QtCore/QDebug>

#include "tlen-avatar-service.h"
#include "misc/path-conversion.h"
#include "server/tlen-avatar-fetcher.h"

void TlenAvatarService::fetchAvatar(ContactAccountData *contactAccountData)
{
	if (contactAccountData->id().isEmpty())
		return;

	if (InProgress.contains(contactAccountData))
		return;

	InProgress.append(contactAccountData);

	TlenAvatarFetcher *avatarFetcher = new TlenAvatarFetcher(contactAccountData, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(ContactAccountData *, const QByteArray &)),
			this, SIGNAL(avatarReady(ContactAccountData *, const QByteArray &)));

	avatarFetcher->fetchAvatar();
	qDebug() << "Tlen Get Avatar" << contactAccountData->id();
}

void TlenAvatarService::avatarReady(ContactAccountData *contactAccountData, const QByteArray &avatar)
{
	InProgress.removeAll(contactAccountData);

	if (avatar.isNull())
		return;

	emit avatarFetched(contactAccountData, avatar);
	qDebug() << "Tlen Have Avatar" << contactAccountData->id();
}
