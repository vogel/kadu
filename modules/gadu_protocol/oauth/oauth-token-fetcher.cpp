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
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

#include "oauth/oauth-parameters.h"

#include "oauth-token-fetcher.h"

QString OAuthTokenFetcher::createUniqueNonce()
{
	return QCA::InitializationVector(16).toByteArray().toHex();
}

QString OAuthTokenFetcher::createTimestamp()
{
	return QString::number(QDateTime::currentDateTime().toTime_t());
}

OAuthTokenFetcher::OAuthTokenFetcher(QString requestTokenUrl, OAuthConsumer consumer, QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject(parent), RequestTokenUrl(requestTokenUrl), Consumer(consumer), NetworkAccessManager(networkAccessManager), Reply(0)
{
}

OAuthTokenFetcher::~OAuthTokenFetcher()
{
}

void OAuthTokenFetcher::fetchToken()
{
	if (!QCA::isSupported("hmac(sha1)"))
	{
		emit tokenFetched(OAuthToken());
		return;
	}

	QStringList signatureBaseItems;
	signatureBaseItems.append("POST"); // the only supported method
	signatureBaseItems.append(RequestTokenUrl.toLocal8Bit().toPercentEncoding());

	OAuthParameters parameters;
	parameters.setConsumerKey(Consumer.consumerKey());
	parameters.setSignatureMethod("HMAC-SHA1");
	parameters.setNonce(createUniqueNonce());
	parameters.setTimestamp(createTimestamp());
	parameters.setVerison("1.0");

	signatureBaseItems.append(parameters.toSignatureBase());

	QByteArray key(Consumer.consumerSecret().toLocal8Bit() + "&");

	QCA::MessageAuthenticationCode hmac("hmac(sha1)", QCA::SymmetricKey(key));
	QCA::SecureArray array(signatureBaseItems.join("&").toLocal8Bit());
	hmac.update(array);

	QByteArray digest = hmac.final().toByteArray().toBase64();
	parameters.setSignature(digest);

	QNetworkRequest request;
	request.setUrl(RequestTokenUrl);
	request.setRawHeader("Connection", "close");
	request.setRawHeader("Content-Length", 0);
	request.setRawHeader("Accept", "text/xml");
	request.setRawHeader("Authorization", parameters.toAuthorizationHeader().toLatin1());

	Reply = NetworkAccessManager->post(request, QByteArray());
	connect(Reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void OAuthTokenFetcher::requestFinished()
{
	if (!Reply)
	{
		OAuthToken token;
		token.setConsumer(Consumer);
		emit tokenFetched(token);
		return;
	}

	if (QNetworkReply::NoError != Reply->error())
	{
		OAuthToken token;
		token.setConsumer(Consumer);
		emit tokenFetched(token);
		return;
	}

	QByteArray xmlContent = Reply->readAll();
	QDomDocument document;
	document.setContent(xmlContent);

	if (document.isNull())
	{
		OAuthToken token;
		token.setConsumer(Consumer);
		emit tokenFetched(token);
		return;
	}

	QDomElement resultElement = document.documentElement();
	QDomElement oauthTokenElement = resultElement.firstChildElement("oauth_token");
	QDomElement oauthTokenSecretElement = resultElement.firstChildElement("oauth_token_secret");
	QDomElement oauthTokenExpiresInlement = resultElement.firstChildElement("oauth_token_expires_in");
	QDomElement statusElement = resultElement.firstChildElement("status");

	if (resultElement.isNull() || oauthTokenElement.isNull() || oauthTokenSecretElement.isNull() || oauthTokenExpiresInlement.isNull() || statusElement.isNull())
	{
		OAuthToken token;
		token.setConsumer(Consumer);
		emit tokenFetched(token);
		return;
	}

	if ("0" != statusElement.text())
	{
		OAuthToken token;
		token.setConsumer(Consumer);
		emit tokenFetched(token);
		return;
	}

	OAuthToken token(oauthTokenElement.text(), oauthTokenSecretElement.text(), oauthTokenExpiresInlement.text().toUInt());
	token.setConsumer(Consumer);
	emit tokenFetched(token);

	deleteLater();
}
