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

#include <QtCrypto/QtCrypto>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>

#include "oauth/oauth-parameters.h"

#include "oauth-manager.h"

OAuthManager::OAuthManager(QObject *parent) :
		QObject(parent)
{
}

OAuthManager::~OAuthManager()
{
}

QString OAuthManager::createUniqueNonce()
{
	return QCA::InitializationVector(16).toByteArray().toHex();
}

QString OAuthManager::createTimestamp()
{
	return QString::number(QDateTime::currentDateTime().toTime_t());
}
#include <stdio.h>
void OAuthManager::fetchToken(QString requestTokenUrl, OAuthConsumer consumer)
{
	if(!QCA::isSupported("hmac(sha1)"))
	{
		emit tokenFetched(OAuthToken());
		return;
	}

	QStringList signatureBaseItems;
	signatureBaseItems.append("POST"); // the only supported method
	signatureBaseItems.append(requestTokenUrl.toLocal8Bit().toPercentEncoding());

	OAuthParameters parameters;
	parameters.setConsumerKey(consumer.consumerKey());
	parameters.setSignatureMethod("HMAC-SHA1");
	parameters.setNonce(createUniqueNonce());
	parameters.setTimestamp(createTimestamp());
	parameters.setVerison("1.0");

	signatureBaseItems.append(parameters.toSignatureBase());

	QByteArray key(consumer.consumerSecret().toLocal8Bit() + "&");

	printf("key: %s\n", key.data());
	printf("signature: %s\n", signatureBaseItems.join("&").toLocal8Bit().data());

	QCA::MessageAuthenticationCode hmac("hmac(sha1)", QCA::SymmetricKey(key));
	QCA::SecureArray array(signatureBaseItems.join("&").toLocal8Bit());
	hmac.update(array);

	QByteArray digest = hmac.final().toByteArray().toBase64();
	printf("digest: %s\n", digest.data());
	parameters.setSignature(digest);

	QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);

	QNetworkRequest request;
	request.setUrl(requestTokenUrl);
	request.setRawHeader("Connection", "close");
	request.setRawHeader("Content-Length", 0);
	request.setRawHeader("Accept", "text/xml");
	request.setRawHeader("Authorization", parameters.toAuthorizationHeader().toLatin1());

	networkManager->post(request, QByteArray());
}

