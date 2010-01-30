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

#include <QtCore/QUuid>
#include <QtCrypto/QtCrypto>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "oauth/oauth-authorization.h"
#include "oauth/oauth-token-fetcher.h"

#include "oauth-manager.h"
#include <QImage>
#include <QMimeData>
#include "oauth-parameters.h"
#include <QFile>

OAuthManager::OAuthManager(QObject *parent) :
		QObject(parent)
{
	NetworkManager = new QNetworkAccessManager(this);
}

OAuthManager::~OAuthManager()
{
}

void OAuthManager::fetchToken(QString requestTokenUrl, OAuthConsumer consumer)
{
	OAuthTokenFetcher *tokenFetcher = new OAuthTokenFetcher(requestTokenUrl, consumer, NetworkManager, this);
	connect(tokenFetcher, SIGNAL(tokenFetched(OAuthToken)), this, SLOT(tokenFetchedSlot(OAuthToken)));
	tokenFetcher->fetchToken();
}

void OAuthManager::tokenFetchedSlot(OAuthToken token)
{
	if (!token.isValid())
		return;

	OAuthAuthorization *authorization = new OAuthAuthorization(token, "https://login.gadu-gadu.pl/authorize", "http://www.mojageneracja.pl",
			token.consumer(), NetworkManager, this);
	connect(authorization, SIGNAL(authorized(OAuthToken, bool)), this, SLOT(authorizedSlot(OAuthToken, bool)));
	authorization->authorize();
}

void OAuthManager::authorizedSlot(OAuthToken token, bool ok)
{
	if (!ok)
		return;

	OAuthTokenFetcher *tokenFetcher = new OAuthTokenFetcher("http://api.gadu-gadu.pl/access_token", token, NetworkManager, this);
	connect(tokenFetcher, SIGNAL(tokenFetched(OAuthToken)), this, SLOT(accessTokenFetchedSlot(OAuthToken)));
	tokenFetcher->fetchToken();
}

void OAuthManager::accessTokenFetchedSlot(OAuthToken token)
{
	QString boundary = QString("-----------------------------") + QUuid::createUuid().toString().remove("{").remove("}").remove("-");
	QFile image("/home/vogel/your-accounts-1.png");
	image.open(QIODevice::ReadOnly);

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
	payload += "Content-Disposition: form-data; name=\"datafile\"; filename=\"avatar.png\"\r\n";
	payload += "Content-Type: image/png\r\n";
	payload += "\r\n";
	payload += image.readAll();
	payload += "\r\n";
	payload += "--";
	payload += boundary.toAscii();
	payload += "--\r\n";

	QNetworkRequest putAvatarRequest;
	putAvatarRequest.setUrl(url);
	putAvatarRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=%1").arg(boundary));

	QStringList signatureBaseItems;
	signatureBaseItems.append("POST"); // the only supported method
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

	NetworkManager->post(putAvatarRequest, payload);
}
