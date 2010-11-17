/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#include "server/jabber-avatar-pep-uploader.h"
#include "server/jabber-avatar-vcard-uploader.h"
#include "jabber-protocol.h"

#include "jabber-avatar-uploader.h"


JabberAvatarUploader::JabberAvatarUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
}

JabberAvatarUploader::~JabberAvatarUploader()
{
}

void JabberAvatarUploader::uploadAvatarPEP(QImage avatar)
{
	JabberAvatarPepUploader *pepUploader = new JabberAvatarPepUploader(MyAccount, this);
	connect(pepUploader, SIGNAL(avatarUploaded(bool, QImage)), this, SIGNAL(avatarUploaded(bool, QImage)));
	pepUploader->uploadAvatar(avatar);
}

void JabberAvatarUploader::uploadAvatarVCard(QImage avatar)
{
	JabberAvatarVCardUploader *vcardUploader = new JabberAvatarVCardUploader(MyAccount, this);
	connect(vcardUploader, SIGNAL(avatarUploaded(bool, QImage)), this, SIGNAL(avatarUploaded(bool, QImage)));
	vcardUploader->uploadAvatar(avatar);

}

void JabberAvatarUploader::uploadAvatar(QImage avatar)
{
	JabberProtocol *protocol = dynamic_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!protocol)
		return;

	if (protocol->isPEPAvailable() && protocol->pepManager())
		uploadAvatarPEP(avatar);
	else
		uploadAvatarVCard(avatar);
}
