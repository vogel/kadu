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

#ifndef OAUTH_AUTHORIZATION_CHAIN_H
#define OAUTH_AUTHORIZATION_CHAIN_H

#include <QtCore/QObject>

#include "oauth/oauth-consumer.h"
#include "oauth/oauth-token.h"

class QNetworkAccessManager;

class OAuthAuthorizationChain : public QObject
{
	Q_OBJECT

	QNetworkAccessManager *NetworkAccessManager;
	OAuthConsumer Consumer;
	OAuthToken Token;
	OAuthToken AccessToken;
	QString RequestTokenUrl;
	QString AuthorizeUrl;
	QString AuthorizeCallbackUrl;
	QString AccessTokenUrl;

private slots:
	void requestTokenFetched(OAuthToken token);
	void authorized(bool ok);
	void accessTokenFetched(OAuthToken token);

public:
	explicit OAuthAuthorizationChain(OAuthConsumer consumer, QNetworkAccessManager *networkAccessManager, QObject *parent = 0);
	virtual ~OAuthAuthorizationChain();

	void setRequestTokenUrl(const QString &requestTokenUrl);
	void setAuthorizeUrl(const QString &authorizeUrl);
	void setAuthorizeCallbackUrl(const QString &authorizeCallbackUrl);
	void setAccessTokenUrl(const QString &accessTokenUrl);

	void authorize();

signals:
	void authorized(OAuthToken token);

};

#endif // OAUTH_AUTHORIZATION_CHAIN_H
