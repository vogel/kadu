/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#include <xmpp_vcard.h>

#include "server/jabber-avatar-uploader.h"
#include "services/jabber-vcard-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-vcard-uploader.h"


#define NS_METADATA "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata"
#define NS_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"
#define MAX_AVATAR_DIMENSION 96

JabberAvatarVCardUploader::JabberAvatarVCardUploader(XMPP::JabberVCardService *vcardService, QObject *parent) :
		AvatarUploader(parent), VCardService(vcardService)
{
}

JabberAvatarVCardUploader::~JabberAvatarVCardUploader()
{
}

void JabberAvatarVCardUploader::done()
{
	emit avatarUploaded(true, UploadedAvatar);
	deleteLater();
}

void JabberAvatarVCardUploader::failed()
{
	emit avatarUploaded(false, UploadedAvatar);
	deleteLater();
}

void JabberAvatarVCardUploader::uploadAvatar(const QString &id, const QString &password, QImage avatar)
{
	Q_UNUSED(password)

	MyJid = id;
	UploadedAvatar = avatar;

	if (!VCardService)
	{
		failed();
		return;
	}

	VCardService.data()->fetch(MyJid, this);
}

void JabberAvatarVCardUploader::vCardFetched(bool ok, const XMPP::VCard &vCard)
{
	if (!ok || !VCardService || !VCardService.data()->xmppClient())
	{
		failed();
		return;
	}

	XMPP::VCard updatedVCard = vCard;
	updatedVCard.setPhoto(JabberAvatarUploader::avatarData(UploadedAvatar));

	VCardService.data()->update(MyJid, updatedVCard, this);
}

void JabberAvatarVCardUploader::vcardUpdated(bool ok)
{
	if (ok)
		done();
	else
		failed();
}
