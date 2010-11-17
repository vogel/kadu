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

#ifndef JABBER_AVATAR_PEP_UPLOADER_H
#define JABBER_AVATAR_PEP_UPLOADER_H

#include <QtGui/QImage>

#include <iris/xmpp_pubsubitem.h>
#include <xmpp/jid/jid.h>

#include "accounts/account.h"

class JabberProtocol;

class JabberAvatarPepUploader : public QObject
{
	Q_OBJECT

	Account MyAccount;
	JabberProtocol *MyProtocol;

	QImage UploadedAvatar;

	QString UploadedAvatarHash;

	// http://xmpp.org/extensions/xep-0153.html
	// we dont like too big files
	QImage createScaledAvatar(const QImage &avatarToScale);

	QByteArray avatarData(const QImage &avatar);

	void doUpload(const QByteArray &data);
	void doRemove();

private slots:
	void publishSuccess(const QString &ns, const XMPP::PubSubItem &item);
	void publishError(const QString &ns, const XMPP::PubSubItem &item);

public:
	explicit JabberAvatarPepUploader(Account account, QObject *parent = 0);
	virtual ~JabberAvatarPepUploader();

	void uploadAvatar(QImage avatar);

signals:
	void avatarUploaded(bool ok, QImage image);

};

#endif // JABBER_AVATAR_UPLOADER_H
