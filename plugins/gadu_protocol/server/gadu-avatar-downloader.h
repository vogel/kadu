/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_AVATAR_DOWNLOADER_H
#define GADU_AVATAR_DOWNLOADER_H

#include <QtGui/QImage>

#include "protocols/services/avatar-downloader.h"

class QNetworkAccessManager;
class QNetworkReply;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class GaduAvatarDownloader
 * @short Class for downloading one avatar for Gadu Gadu protocol.
 * @author Rafał 'Vogel' Malinowski
 *
 * Downloading avatars in Gadu Gadu protocol requires sending one HTTP request. No authorization is required. This
 * class supports up to 5 redirects from accessed url. After that it is assumed that avatar is not available
 * and avatarDownloaded() is emitted with failure flag.
 */
class GaduAvatarDownloader : public AvatarDownloader
{
	Q_OBJECT

	QNetworkAccessManager *NetworkAccessManager;
	QNetworkReply *Reply;
	int RedirectCount;

	void done(QImage avatar);
	void failed();

	void fetch(const QString &url);
	void parseReply();

private slots:
	void requestFinished();

public:
	/**
	 * @short Create new GaduAvatarDownloader instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent
	 */
	explicit GaduAvatarDownloader(QObject *parent = 0);
	virtual ~GaduAvatarDownloader();

	virtual void downloadAvatar(const QString &id);

};

/**
 * @}
 */

#endif // GADU_AVATAR_DOWNLOADER_H
