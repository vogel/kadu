/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCrypto/QtCrypto>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "oauth/oauth-manager.h"
#include "oauth/oauth-parameters.h"
#include "oauth/oauth-token-fetcher.h"

#include "gadu-avatar-uploader.h"

GaduAvatarUploader::GaduAvatarUploader(Account account, QObject *parent) :
		QObject(parent), MyAccount(account)
{
	NetworkAccessManager = new QNetworkAccessManager(this);
	Reply = 0;
}

GaduAvatarUploader::~GaduAvatarUploader()
{

}

void GaduAvatarUploader::uploadAvatar(QImage avatar)
{
	Avatar = avatar;

	OAuthManager *authManager = new OAuthManager(this);
	connect(authManager, SIGNAL(authorized(OAuthToken)), this, SLOT(authorized(OAuthToken)));
	authManager->authorize(OAuthConsumer(MyAccount.id(), MyAccount.password()));
}

void GaduAvatarUploader::authorized(OAuthToken token)
{
	if (!token.isValid())
	{
		emit avatarUploaded(false);
		deleteLater();
	}

	QString boundary = QString("-----------------------------") + QUuid::createUuid().toString().remove("{").remove("}").remove("-");

	QBuffer avatarBuffer;
	avatarBuffer.open(QIODevice::WriteOnly);
	Avatar.save(&avatarBuffer, "PNG");
	avatarBuffer.close();

	QString url = QString("http://api.gadu-gadu.pl/avatars/%1/0.xml").arg(token.consumer().consumerKey());

	QByteArray payload;
	payload += "--";
	payload += boundary.toAscii();
	payload += "\r\n";
	payload += "Content-Disposition: form-data; name=\"_method\"\r\n";
	payload += "\r\n";
	payload += "PUT\r\n";
	payload += "--";
	payload += boundary.toAscii();
	payload += "\r\n";
	payload += "Content-Disposition: form-data; name=\"avatar\"; filename=\"avatar.png\"\r\n";
	payload += "Content-Type: image/png\r\n";
	payload += "\r\n";
	payload += avatarBuffer.buffer();
	payload += "\r\n";
	payload += "--";
	payload += boundary.toAscii();
	payload += "--\r\n";

	QNetworkRequest putAvatarRequest;
	putAvatarRequest.setUrl(url);
	putAvatarRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=%1").arg(boundary));

	QStringList signatureBaseItems;
	signatureBaseItems.append("PUT"); // the only supported method
	signatureBaseItems.append(url.toLocal8Bit().toPercentEncoding());

	OAuthParameters parameters;
	parameters.setConsumerKey(token.consumer().consumerKey());
	parameters.setSignatureMethod("HMAC-SHA1");
	parameters.setNonce(OAuthTokenFetcher::createUniqueNonce());
	parameters.setTimestamp(OAuthTokenFetcher::createTimestamp());
	parameters.setVerison("1.0");
	parameters.setToken(token);

	signatureBaseItems.append(parameters.toSignatureBase());

	QByteArray key(token.consumer().consumerSecret().toLocal8Bit() + "&" + token.tokenSecret().toLocal8Bit());

	QCA::MessageAuthenticationCode hmac("hmac(sha1)", QCA::SymmetricKey(key));
	QCA::SecureArray array(signatureBaseItems.join("&").toLocal8Bit());
	hmac.update(array);

	QByteArray digest = hmac.final().toByteArray().toBase64();
	parameters.setSignature(digest);

	putAvatarRequest.setRawHeader("Authorization", parameters.toAuthorizationHeader().toLatin1());

	Reply = NetworkAccessManager->post(putAvatarRequest, payload);
	connect(Reply, SIGNAL(finished()), SLOT(transferFinished()));
}

void GaduAvatarUploader::transferFinished()
{
	emit avatarUploaded(QNetworkReply::NoError == Reply->error());
	deleteLater();
}
