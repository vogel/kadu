/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "protocols/services/avatar-uploader.h"

class QNetworkAccessManager;
class QNetworkReply;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarUploader
 * @short Class for uploading avatar for Gadu Gadu protocol.
 * @see AvatarUploader
 * @author Rafał 'Vogel' Malinowski
 *
 * Uploading avatar in Gadu Gadu protocol is done by sending seriees of HTTP requests that are authenticated with
 * OAuth protocol. User id and password are required to do this operation.
 */
class GaduAvatarUploader : public AvatarUploader
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

	virtual void uploadAvatar(const QString &id, const QString &password, QImage avatar);

};

/**
 * @}
 */

#endif // GADU_AVATAR_UPLOADER_H
