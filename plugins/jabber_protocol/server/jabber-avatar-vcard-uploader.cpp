/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-avatar-vcard-uploader.h"

#include "server/jabber-avatar-uploader.h"
#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "services/jabber-vcard-uploader.h"
#include "jabber-protocol.h"

#include <qxmpp/QXmppVCardIq.h>

JabberAvatarVCardUploader::JabberAvatarVCardUploader(JabberVCardService *vcardService, QObject *parent) :
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
	Q_UNUSED(password);

	UploadedAvatar = avatar;

	if (!VCardService)
	{
		failed();
		return;
	}

	auto vCardDownloader = VCardService.data()->createVCardDownloader();
	if (!vCardDownloader)
	{
		failed();
		return;
	}

	connect(vCardDownloader, SIGNAL(vCardDownloaded(bool,QXmppVCardIq)), this, SLOT(vCardDownloaded(bool,QXmppVCardIq)));
	vCardDownloader->downloadVCard(id);
}

void JabberAvatarVCardUploader::vCardDownloaded(bool ok, const QXmppVCardIq &vcard)
{
	if (!ok || !VCardService)
	{
		failed();
		return;
	}

	auto updatedVCard = vcard;
	updatedVCard.setPhoto(JabberAvatarUploader::avatarData(UploadedAvatar));

	auto vCardUploader = VCardService.data()->createVCardUploader();
	if (!vCardUploader)
	{
		failed();
		return;
	}

	connect(vCardUploader, SIGNAL(vCardUploaded(bool)), this, SLOT(vCardUploaded(bool)));
	vCardUploader->uploadVCard(updatedVCard);
}

void JabberAvatarVCardUploader::vCardUploaded(bool ok)
{
	if (ok)
		done();
	else
		failed();
}

#include "moc_jabber-avatar-vcard-uploader.cpp"
