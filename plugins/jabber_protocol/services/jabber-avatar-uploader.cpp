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

#include "jabber-avatar-uploader.h"

#include "services/jabber-avatar-uploader.h"
#include "services/jabber-vcard-downloader.h"
#include "services/jabber-vcard-service.h"
#include "services/jabber-vcard-uploader.h"
#include "jabber-protocol.h"

#include <QtCore/QBuffer>
#include <qxmpp/QXmppVCardIq.h>

#define MAX_AVATAR_DIMENSION 96

QByteArray JabberAvatarUploader::avatarData(QImage avatar)
{
	if (avatar.height() > MAX_AVATAR_DIMENSION || avatar.width() > MAX_AVATAR_DIMENSION)
		avatar = avatar.scaled(MAX_AVATAR_DIMENSION, MAX_AVATAR_DIMENSION, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QByteArray data;
	QBuffer buffer(&data);
	buffer.open(QIODevice::WriteOnly);
	avatar.save(&buffer, "PNG");
	buffer.close();

	return data;
}

JabberAvatarUploader::JabberAvatarUploader(JabberVCardService *vcardService, QObject *parent) :
		AvatarUploader(parent), VCardService(vcardService)
{
}

JabberAvatarUploader::~JabberAvatarUploader()
{
}

void JabberAvatarUploader::done()
{
	emit avatarUploaded(true, UploadedAvatar);
	deleteLater();
}

void JabberAvatarUploader::failed()
{
	emit avatarUploaded(false, UploadedAvatar);
	deleteLater();
}

void JabberAvatarUploader::uploadAvatar(const QString &id, const QString &password, QImage avatar)
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

void JabberAvatarUploader::vCardDownloaded(bool ok, const QXmppVCardIq &vcard)
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

void JabberAvatarUploader::vCardUploaded(bool ok)
{
	if (ok)
		done();
	else
		failed();
}

#include "moc_jabber-avatar-uploader.cpp"
