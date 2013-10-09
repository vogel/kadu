/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "services/jabber-vcard-uploader.h"
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

	JabberVCardDownloader *vCardDownloader = VCardService.data()->createVCardDownloader();
	if (!vCardDownloader)
	{
		failed();
		return;
	}

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,XMPP::VCard)), this, SLOT(vCardDownloaded(bool,XMPP::VCard)));
	vCardDownloader->downloadVCard(id);
}

void JabberAvatarVCardUploader::vCardDownloaded(bool ok, XMPP::VCard vCard)
{
	if (!ok || !VCardService)
	{
		failed();
		return;
	}

	XMPP::VCard updatedVCard = vCard;
	updatedVCard.setPhoto(JabberAvatarUploader::avatarData(UploadedAvatar));

	JabberVCardUploader *vCardUploader = VCardService.data()->createVCardUploader();
	if (!vCardUploader)
	{
		failed();
		return;
	}

	connect(vCardUploader, SIGNAL(vCardUploaded(bool)), this, SLOT(vCardUploaded(bool)));
	vCardUploader->uploadVCard(MyJid.bare(), updatedVCard);
}

void JabberAvatarVCardUploader::vCardUploaded(bool ok)
{
	if (ok)
		done();
	else
		failed();
}

#include "moc_jabber-avatar-vcard-uploader.cpp"
