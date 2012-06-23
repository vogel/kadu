/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_AVATAR_PEP_UPLOADER_H
#define JABBER_AVATAR_PEP_UPLOADER_H

#include <QtGui/QImage>

#include <iris/xmpp_pubsubitem.h>
#include <xmpp/jid/jid.h>

#include "accounts/account.h"

namespace XMPP
{
	class JabberProtocol;
}

class JabberPepService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarPepUploader
 * @author Rafał 'Vogel' Malinowski
 * @short Uploads avatar to XMPP server using Pep.
 *
 * This class allows for easy upload of avatar to XMPP server. New instance can be created by constructor that requires
 * JabberPepService argument.
 *
 * After creating call uploadAvatar() to send new avatar and wait for avatarUploaded() signal to be emited. This class will
 * delete itself after emiting avatarUploaded() signal.
 */
class JabberAvatarPepUploader : public QObject
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;

	QImage UploadedAvatar;

	QString ItemId;

	void doUpload(const QByteArray &data);
	void doRemove();

private slots:
	void publishSuccess(const QString &ns, const XMPP::PubSubItem &item);
	void publishError(const QString &ns, const XMPP::PubSubItem &item);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given JabberPepService.
	 * @param pepService instance of JabberPepService
	 * @param parent QObject parent
	 */
	explicit JabberAvatarPepUploader(JabberPepService *pepService, QObject *parent);
	virtual ~JabberAvatarPepUploader();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Uploads avatar to server.
	 * @param avatar avatar to upload
	 *
	 * Avatar can be null. In that case, avatar will be deleted from server.
	 * After uploading avatarUploaded() signal is emited and this instance is destroyed.
	 *
	 * This method can be called only once per instance. Behaviour on second call is undefined.
	 */
	void uploadAvatar(const QImage &avatar);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited after upload is finished.
	 * @param ok result of upload
	 *
	 * If ok is true then uploading/deleting avatar was successfull. If not, then it failed.
	 *
	 * After this signal is emited instance is no longer available, as this class destroys itself.
	 */
	void avatarUploaded(bool ok);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_UPLOADER_H
