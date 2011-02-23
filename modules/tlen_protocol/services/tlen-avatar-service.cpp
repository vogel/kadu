/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QDebug>

#include "tlen-avatar-service.h"
#include "misc/path-conversion.h"
#include "server/tlen-avatar-fetcher.h"

void TlenAvatarService::fetchAvatar(Contact contact)
{
	if (contact.id().isEmpty())
		return;

	//if (InProgress.contains(contact))
	//	return;

	//InProgress.append(contact);

	TlenAvatarFetcher *avatarFetcher = new TlenAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact, const QByteArray &)),
			this, SLOT(avatarReady(Contact, const QByteArray &)));
	avatarFetcher->fetchAvatar();
	qDebug() << "Tlen Get Avatar" << contact.id();
}

void TlenAvatarService::uploadAvatar(QImage avatar)
{
	Q_UNUSED(avatar)
}

void TlenAvatarService::avatarReady(Contact contact, const QByteArray &avatar)
{
	InProgress.removeAll(contact);

	if (avatar.isEmpty())
		return;

	emit avatarFetched(contact, avatar);
	qDebug() << "Tlen Have Avatar" << contact.id()  << avatar.size();
}
