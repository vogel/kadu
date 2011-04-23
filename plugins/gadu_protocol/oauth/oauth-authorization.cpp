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
	payback += "callback_url=http://www.mojageneracja.pl&request_token=";
	payback += Token.token();
	payback += "&uin=";
	payback += Token.consumer().consumerKey();
	payback += "&password=";
	payback += Token.consumer().consumerSecret();

	QNetworkRequest request;
	request.setUrl(AuthorizationUrl);
	request.setRawHeader("Connection", "close");
	request.setRawHeader("Content-Length", QByteArray::number(payback.size()));

	Reply = NetworkAccessManager->post(request, payback);
	connect(Reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

void OAuthAuthorization::requestFinished()
{
	emit authorized(QNetworkReply::NoError == Reply->error());
	deleteLater();
}
