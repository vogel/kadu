/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "utils/vcard-factory.h"
#include "jabber-protocol.h"

#include "jabber-avatar-vcard-uploader.h"


#define NS_METADATA "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata"
#define NS_DATA "http://www.xmpp.org/extensions/xep-0084.html#ns-data"
#define MAX_AVATAR_DIMENSION 96

JabberAvatarVCardUploader::JabberAvatarVCardUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
	MyProtocol = dynamic_cast<JabberProtocol *>(account.protocolHandler());
}

JabberAvatarVCardUploader::~JabberAvatarVCardUploader()
{
}

void JabberAvatarVCardUploader::uploadAvatar(const QByteArray &data)
{
	UploadedAvatarData = data;

	VCardFactory::instance()->getVCard(MyAccount.id(), MyProtocol->client()->rootTask(), this, SLOT(vcardReceived()));
}

void JabberAvatarVCardUploader::vcardReceived()
{
	XMPP::JT_VCard *task = dynamic_cast<XMPP::JT_VCard *>(sender());

	if (!task || !task->success())
	{
		emit avatarUploaded(false);
		deleteLater();
		return;
	}

	XMPP::Jid jid = XMPP::Jid(MyAccount.id());

	XMPP::VCard vcard = task->vcard();
	vcard.setPhoto(UploadedAvatarData);

	VCardFactory::instance()->setVCard(MyProtocol->client()->rootTask(), jid, vcard, this, SLOT(vcardUploaded()));
}

void JabberAvatarVCardUploader::vcardUploaded()
{
	XMPP::JT_VCard *task = dynamic_cast<XMPP::JT_VCard *>(sender());
	if (!task || !task->success())
	{
		emit avatarUploaded(false);
		deleteLater();
		return;
	}


	emit avatarUploaded(true);
	deleteLater();

	printf("vcard uploaded\n");
}
