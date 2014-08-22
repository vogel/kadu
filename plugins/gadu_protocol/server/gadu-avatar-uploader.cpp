/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QBuffer>
#include <QtCore/QRegExp>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "oauth/oauth-manager.h"
#include "oauth/oauth-token-fetcher.h"

#include "gadu-avatar-uploader.h"

GaduAvatarUploader::GaduAvatarUploader(QObject *parent) :
		AvatarUploader(parent)
{
	NetworkAccessManager = new QNetworkAccessManager(this);
	Reply = 0;
}

GaduAvatarUploader::~GaduAvatarUploader()
{

}

void GaduAvatarUploader::uploadAvatar(const QString &id, const QString &password, QImage avatar)
{
	Id = id;
	Avatar = avatar;

	OAuthManager *authManager = new OAuthManager(this);
	connect(authManager, SIGNAL(authorized(OAuthToken)), this, SLOT(authorized(OAuthToken)));
	authManager->authorize(OAuthConsumer(id.toUtf8(), password.toUtf8()));
}

void GaduAvatarUploader::authorized(OAuthToken token)
{
	if (!token.isValid())
	{
		emit avatarUploaded(false, Avatar);
		deleteLater();
		return;
	}

	QBuffer avatarBuffer;
	avatarBuffer.open(QIODevice::WriteOnly);
	Avatar.save(&avatarBuffer, "PNG");
	avatarBuffer.close();

	QByteArray url;
	url += "http://avatars.nowe.gg/upload";

	QByteArray payload;
	payload += "uin=" + QUrl::toPercentEncoding(Id);
	payload += "&photo=";
	payload += QUrl::toPercentEncoding(avatarBuffer.buffer().toBase64());

	QNetworkRequest putAvatarRequest;
	putAvatarRequest.setUrl(QString(url));
	putAvatarRequest.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/x-www-form-urlencoded"));

	putAvatarRequest.setRawHeader("Authorization", token.token());
	putAvatarRequest.setRawHeader("From", "avatars to avatars");

	Reply = NetworkAccessManager->post(putAvatarRequest, payload);
	connect(Reply, SIGNAL(finished()), SLOT(transferFinished()));
}

void GaduAvatarUploader::transferFinished()
{
	emit avatarUploaded(QNetworkReply::NoError == Reply->error(), Avatar);
	deleteLater();
}

#include "moc_gadu-avatar-uploader.cpp"
