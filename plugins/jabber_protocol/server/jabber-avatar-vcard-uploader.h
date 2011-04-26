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

#ifndef JABBER_AVATAR_VCARD_UPLOADER_H
#define JABBER_AVATAR_VCARD_UPLOADER_H

#include <QtGui/QImage>

#include <iris/xmpp_pubsubitem.h>
#include <xmpp/jid/jid.h>

#include "accounts/account.h"

class JabberProtocol;

class JabberAvatarVCardUploader : public QObject
{
	Q_OBJECT

	Account MyAccount;
	JabberProtocol *MyProtocol;

	QByteArray UploadedAvatarData;

private slots:
	void vcardReceived();
	void vcardUploaded();

public:
	explicit JabberAvatarVCardUploader(Account account, QObject *parent = 0);
	virtual ~JabberAvatarVCardUploader();

	void uploadAvatar(const QByteArray &data);

signals:
	void avatarUploaded(bool ok);

};

#endif // JABBER_AVATAR_VCARD_UPLOADER_H
