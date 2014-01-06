/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtGui/QImage>

#include "protocols/services/avatar-uploader.h"

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
 * @short Uploads avatar to XMPP server using PEP or VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy upload of avatar to XMPP server. New instance can be created by constructor that requires
 * JabberPepService and XMPP::JabberVCardService arguments. If both services are null then this class will always fail
 * to do it job. If one is provided then it will be used to upload avatar. If both are provided then PEP service will
 * be used and VCard only when PEP service is not enabled or when it failed.
 *
 * This class internally used JabberAvatarPepUploader and JabberAvatarVCardUploader.
 */
class JabberAvatarUploader : public AvatarUploader
{
	Q_OBJECT

	QPointer<JabberPepService> PepService;
	QPointer<XMPP::JabberVCardService> VCardService;

	QString Id;
	QString Password;
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
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return image data as PNG byte array.
	 * @param avatar avatar image to convert
	 * @return image data as PNG byte array
	 */
	static QByteArray avatarData(const QImage &avatar);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given services.
	 * @param pepService instance of JabberPepService
	 * @param vCardService instance of XMPP::JabberVCardService
	 * @param parent QObject parent
	 */
	JabberAvatarUploader(JabberPepService *pepService, XMPP::JabberVCardService *vCardService, QObject *parent);
	virtual ~JabberAvatarUploader();

	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar);

};

/**
 * @}
 */

#endif // JABBER_AVATAR_UPLOADER_H
