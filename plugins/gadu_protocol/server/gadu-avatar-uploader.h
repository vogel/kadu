/*
 * %kadu copyright begin%
 * Copyright 2008 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef GADU_AVATAR_UPLOADER_H
#define GADU_AVATAR_UPLOADER_H

#include <QtGui/QImage>

#include "oauth/oauth-token.h"

class QNetworkAccessManager;
class QNetworkReply;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarUploader
 * @short Class for uploading avatar for Gadu Gadu protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class allows for uploading avatar for Gadu Gadu protocol. To do that attach slot to avatarUploaded()
 * signal and call uploadAvatar() method. After avatar is downloaded avatarUploaded() signal is emitted and this
 * object deletes itself.
 *
 * Fetching avatars in Gadu Gadu protocol is done by sending seriees of HTTP requests that are authenticated without
 * OAuth protocol. Providing user id and password is required to do this operation.
 *
 * Signal avatarUploaded() is emitted with success flag and uploaded avatar image.
 */
class GaduAvatarUploader : public QObject
{
	Q_OBJECT

	QString Id;
	QNetworkAccessManager *NetworkAccessManager;
	QNetworkReply *Reply;

	QImage Avatar;

private slots:
	void authorized(OAuthToken token);
	void transferFinished();

public:
	/**
	 * @short Create new GaduAvatarUploader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit GaduAvatarUploader(QObject *parent = 0);
	virtual ~GaduAvatarUploader();

	/**
	 * @short Uploads avatar with given authentication data.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id of contact to upload avatar for
	 * @param password password of contact to upload avatar for
	 * @param avatar avatar to upload
	 *
	 * Before calling this method attach to avatarUploaded() signal to get informed about result. Please
	 * note that this method can be only called once. After that this object emits avatarUploaded() and
	 * deletes itself.
	 */
	void uploadAvatar(const QString &id, const QString &password, QImage avatar);

signals:
	/**
	 * @short Signal emitted when job of this class is done.
	 * @author Rafał 'Vogel' Malinowski
	 * @param ok success flag
	 * @param avatar uploaded avatar
	 *
	 * If ok is true then avatar uploading was successfull. If ok is false then operation failed.
	 * Second parameter is always the same as avatar passed to uploadAvatar() method.
	 */
	void avatarUploaded(bool ok, QImage avatar);

};

/**
 * @}
 */

#endif // GADU_AVATAR_UPLOADER_H
