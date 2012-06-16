/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QBuffer>

#include <xmpp_client.h>

#include "server/jabber-avatar-pep-uploader.h"
#include "server/jabber-avatar-vcard-uploader.h"
#include "services/jabber-pep-service.h"
#include "jabber-protocol.h"

#include "jabber-avatar-uploader.h"

#define MAX_AVATAR_DIMENSION 96

QByteArray JabberAvatarUploader::avatarData(const QImage &avatar)
{
	QByteArray data;
	QBuffer buffer(&data);
	buffer.open(QIODevice::WriteOnly);
	avatar.save(&buffer, "PNG");
	buffer.close();

	return data;
}

JabberAvatarUploader::JabberAvatarUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
}

JabberAvatarUploader::~JabberAvatarUploader()
{
}

QImage JabberAvatarUploader::createScaledAvatar(const QImage &avatarToScale)
{
	if (avatarToScale.height() < MAX_AVATAR_DIMENSION && avatarToScale.width() < MAX_AVATAR_DIMENSION)
		return avatarToScale;

	return avatarToScale.scaled(MAX_AVATAR_DIMENSION, MAX_AVATAR_DIMENSION, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void JabberAvatarUploader::uploadAvatarPEP()
{
	JabberAvatarPepUploader *pepUploader = JabberAvatarPepUploader::createForAccount(MyAccount, this);
	if (!pepUploader)
		return;

	connect(pepUploader, SIGNAL(avatarUploaded(bool)), this, SLOT(pepAvatarUploaded(bool)));
	pepUploader->uploadAvatar(UploadingAvatar);
}

void JabberAvatarUploader::uploadAvatarVCard()
{
	JabberAvatarVCardUploader *vcardUploader = new JabberAvatarVCardUploader(MyAccount, this);
	connect(vcardUploader, SIGNAL(avatarUploaded(bool)), this, SLOT(avatarUploadedSlot(bool)));
	vcardUploader->uploadAvatar(UploadingAvatar);
}

void JabberAvatarUploader::pepAvatarUploaded(bool ok)
{
	if (ok)
	{
		emit avatarUploaded(ok, UploadingAvatar);
		deleteLater();
		return;
	}

	// do a fallback to vcard
	uploadAvatarVCard();
}

void JabberAvatarUploader::avatarUploadedSlot(bool ok)
{
	emit avatarUploaded(ok, UploadingAvatar);
	deleteLater();
}

void JabberAvatarUploader::uploadAvatar(QImage avatar)
{
	XMPP::JabberProtocol *protocol = qobject_cast<XMPP::JabberProtocol *>(MyAccount.protocolHandler());
	if (!protocol || !protocol->xmppClient() || !protocol->xmppClient()->rootTask())
	{
		deleteLater();
		emit avatarUploaded(false, avatar);
		return;
	}

	UploadingAvatar = createScaledAvatar(avatar);

	if (protocol->pepService()->enabled())
		uploadAvatarPEP();
	else
		uploadAvatarVCard();
}
