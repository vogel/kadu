/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtXml/QDomDocument>

#include "misc/path-conversion.h"
#include "server/gadu-avatar-fetcher.h"
#include "server/gadu-avatar-uploader.h"
#include "gadu-avatar-service.h"

void GaduAvatarService::fetchAvatar(Contact contact)
{
	if (contact.id().isEmpty())
		return;

	GaduAvatarFetcher *avatarFetcher = new GaduAvatarFetcher(contact, this);
	connect(avatarFetcher, SIGNAL(avatarFetched(Contact, bool)),
			this, SIGNAL(avatarFetched(Contact, bool)));
	avatarFetcher->fetchAvatar();
}

void GaduAvatarService::uploadAvatar(QImage avatar)
{
	if (account().accountContact().id().isEmpty())
		return;

	GaduAvatarUploader *avatarUploader = new GaduAvatarUploader(account(), this);
	connect(avatarUploader, SIGNAL(avatarUploaded(bool, QImage)), this, SIGNAL(avatarUploaded(bool, QImage)));
	avatarUploader->uploadAvatar(avatar);
}
