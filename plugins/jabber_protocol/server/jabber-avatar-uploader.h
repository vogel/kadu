/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_UPLOADER_H
#define JABBER_AVATAR_UPLOADER_H

#include <QtCore/QWeakPointer>
#include <QtGui/QImage>

class QNetworkAccessManager;
class QNetworkReply;

namespace XMPP { class JabberVCardService; }

class JabberPepService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarUploader
 * @author Rafał 'Vogel' Malinowski
 * @short Uploads avatar to XMPP server using VCard or Pep.
 *
 * This class allows for easy upload of avatar to XMPP server. New instance can be created by constructor that requires
 * JabberPepService and XMPP::JabberVCardService arguments. If both services are null then this class will always fail
 * to do it job. If one is provided then it will be used to upload avatar. If both are provided then PEP service will
 * be used and VCard only when PEP service is not enabled or when it failed.
 *
 * This class internally used JabberAvatarPepUploader and JabberAvatarVCardUploader.
 *
 * After creating call uploadAvatar() to send new avatar and wait for avatarUploaded() signal to be emited. This class will
 * delete itself after emiting avatarUploaded() signal.
 */
class JabberAvatarUploader : public QObject
{
	Q_OBJECT

	QWeakPointer<JabberPepService> PepService;
	QWeakPointer<XMPP::JabberVCardService> VCardService;

	QString Id;
	QImage UploadingAvatar;

	// http://xmpp.org/extensions/xep-0153.html
	// we dont like too big files
	QImage createScaledAvatar(const QImage &avatarToScale);

	void uploadAvatarPEP();
	void uploadAvatarVCard();

private slots:
	void pepAvatarUploaded(bool ok);
	void avatarUploadedSlot(bool ok);

public:
	static QByteArray avatarData(const QImage &avatar);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given JabberAvatarUploader.
	 * @param pepService instance of JabberPepService
	 * @param vCardService instance of XMPP::JabberVCardService
	 * @param parent QObject parent
	 */
	JabberAvatarUploader(JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent);
	virtual ~JabberAvatarUploader();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Uploads avatar to server.
	 * @param jid jid of account owner
	 * @param avatar avatar to upload
	 *
	 * Avatar can be null. In that case, avatar will be deleted from server.
	 * After uploading avatarUploaded() signal is emited and this instance is destroyed.
	 *
	 * This method can be called only once per instance. Behaviour on second call is undefined.
	 */
	void uploadAvatar(const QString &id, QImage avatar);

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
	void avatarUploaded(bool ok, QImage image);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_UPLOADER_H
