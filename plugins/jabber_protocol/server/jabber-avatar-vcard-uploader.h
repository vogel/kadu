/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_AVATAR_VCARD_UPLOADER_H
#define JABBER_AVATAR_VCARD_UPLOADER_H

#include <QtGui/QImage>

#include "accounts/account.h"
#include "protocols/services/avatar-uploader.h"

class JabberVCardService;

/**
 * @addtogroup Jabber
 * @{
 */

/**
 * @class JabberAvatarVCardUploader
 * @short Uploads avatar to XMPP server using VCard.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for easy upload of avatar to XMPP server. New instance can be created by constructor that requires
 * JabberVCardService argument.
 */
class JabberAvatarVCardUploader : public AvatarUploader
{
	Q_OBJECT

	//Jid MyJid;
	QPointer<JabberVCardService> VCardService;

	QImage UploadedAvatar;

	void done();
	void failed();

private slots:
	void vCardUploaded(bool ok);
	// void vCardDownloaded(bool ok, VCard vCard);

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance attached to given JabberVCardService.
	 * @param vcardService instance of JabberVCardService
	 * @param parent QObject parent
	 */
	explicit JabberAvatarVCardUploader(JabberVCardService *vcardService, QObject *parent = 0);
	virtual ~JabberAvatarVCardUploader();

	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar);

};

#endif // JABBER_AVATAR_VCARD_UPLOADER_H
