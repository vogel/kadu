/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "server/jabber-avatar-fetcher.h"
#include "server/jabber-avatar-uploader.h"
#include "services/jabber-pep-service.h"
#include "services/jabber-vcard-service.h"

#include "jabber-avatar-service.h"

JabberAvatarService::JabberAvatarService(QObject *parent) :
		AvatarService(parent)
{
}

JabberAvatarService::~JabberAvatarService()
{
}

void JabberAvatarService::setPepService(JabberPepService *pepService)
{
	PepService = pepService;
}

void JabberAvatarService::setVCardService(XMPP::JabberVCardService *vCardService)
{
	VCardService = vCardService;
}

void JabberAvatarService::fetchAvatar(const QString &id, QObject *receiver)
{
	if (id.isEmpty())
		return;

	JabberAvatarFetcher *avatarFetcher = new JabberAvatarFetcher(id, PepService.data(), VCardService.data(), this);
	connect(avatarFetcher, SIGNAL(avatarFetched(bool,QPixmap)),
			receiver, SLOT(avatarFetched(bool,QPixmap)));
	avatarFetcher->fetchAvatar();
}

void JabberAvatarService::uploadAvatar(const QString &id, const QString &password, QImage avatar)
{
	if (id.isEmpty())
		return;

	JabberAvatarUploader *avatarUploader = new JabberAvatarUploader(PepService.data(), VCardService.data(), this);
	connect(avatarUploader, SIGNAL(avatarUploaded(bool, QImage)), this, SIGNAL(avatarUploaded(bool, QImage)));
	avatarUploader->uploadAvatar(id, password, avatar);
}
