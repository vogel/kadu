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

void TlenAvatarService::fetchAvatar(Contact *contact)
{
	if (contact->id().isEmpty())
		return;

	if (InProgress.contains(contact))
		return;

	InProgress.append(contact);

	TlenAvatarFetcher *avatarFetcher = new TlenAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact *, const QByteArray &)),
			this, SIGNAL(avatarReady(Contact *, const QByteArray &)));

	avatarFetcher->fetchAvatar();
	qDebug() << "Tlen Get Avatar" << contact->id();
}

void TlenAvatarService::avatarReady(Contact *contact, const QByteArray &avatar)
{
	InProgress.removeAll(contact);

	if (avatar.isNull())
		return;

	emit avatarFetched(contact, avatar);
	qDebug() << "Tlen Have Avatar" << contact->id();
}
