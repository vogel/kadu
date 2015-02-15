/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "oauth-authorization.h"

OAuthAuthorization::OAuthAuthorization(OAuthToken token, const QString &authorizationUrl, const QString &callbackUrl, OAuthConsumer consumer,
		QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject(parent), Token(token), AuthorizationUrl(authorizationUrl),
		CallbackUrl(callbackUrl), Consumer(consumer), NetworkAccessManager(networkAccessManager), Reply(0)
{
}

OAuthAuthorization::~OAuthAuthorization()
{
}

void OAuthAuthorization::authorize()
{
	QByteArray payback;
	payback += "callback_url=";
	payback += QUrl::toPercentEncoding("http://www.mojageneracja.pl");
	payback += "&request_token=";
	payback += QUrl::toPercentEncoding(Token.token());
	payback += "&uin=";
	payback += QUrl::toPercentEncoding(Token.consumer().consumerKey());
	payback += "&password=";
	payback += QUrl::toPercentEncoding(Token.consumer().consumerSecret());

	QNetworkRequest request;
	request.setUrl(AuthorizationUrl);
	request.setRawHeader("Connection", "close");
	request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(payback.size()));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	Reply = NetworkAccessManager->post(request, payback);
	connect(Reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void OAuthAuthorization::requestFinished()
{
	emit authorized(QNetworkReply::NoError == Reply->error());
	deleteLater();
}

#include "moc_oauth-authorization.cpp"
