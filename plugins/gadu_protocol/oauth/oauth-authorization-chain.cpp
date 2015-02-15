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

#include "oauth/oauth-authorization.h"
#include "oauth/oauth-token-fetcher.h"

#include "oauth-authorization-chain.h"

OAuthAuthorizationChain::OAuthAuthorizationChain(OAuthConsumer consumer, QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject(parent), NetworkAccessManager(networkAccessManager), Consumer(consumer)
{
}

OAuthAuthorizationChain::~OAuthAuthorizationChain()
{
}

void OAuthAuthorizationChain::setRequestTokenUrl(const QString &requestTokenUrl)
{
	RequestTokenUrl = requestTokenUrl;
}

void OAuthAuthorizationChain::setAuthorizeUrl(const QString &authorizeUrl)
{
	AuthorizeUrl = authorizeUrl;
}

void OAuthAuthorizationChain::setAuthorizeCallbackUrl(const QString &authorizeCallbackUrl)
{
	AuthorizeCallbackUrl = authorizeCallbackUrl;
}

void OAuthAuthorizationChain::setAccessTokenUrl(const QString &accessTokenUrl)
{
	AccessTokenUrl = accessTokenUrl;
}

void OAuthAuthorizationChain::authorize()
{
	OAuthTokenFetcher *tokenFetcher = new OAuthTokenFetcher(RequestTokenUrl, Consumer, NetworkAccessManager, this);
	connect(tokenFetcher, SIGNAL(tokenFetched(OAuthToken)), this, SLOT(requestTokenFetched(OAuthToken)));
	tokenFetcher->fetchToken();
}

void OAuthAuthorizationChain::requestTokenFetched(OAuthToken token)
{
	Token = token;

	if (!Token.isValid())
	{
		emit authorized(AccessToken);
		deleteLater();
		return;
	}

	OAuthAuthorization *authorization = new OAuthAuthorization(Token, AuthorizeUrl, AuthorizeCallbackUrl, Consumer, NetworkAccessManager, this);
	connect(authorization, SIGNAL(authorized(bool)), this, SLOT(authorized(bool)));
	authorization->authorize();
}

void OAuthAuthorizationChain::authorized(bool ok)
{
	if (!ok)
	{
		emit authorized(AccessToken);
		deleteLater();
		return;
	}

	OAuthTokenFetcher *tokenFetcher = new OAuthTokenFetcher(AccessTokenUrl, Token, NetworkAccessManager, this);
	connect(tokenFetcher, SIGNAL(tokenFetched(OAuthToken)), this, SLOT(accessTokenFetched(OAuthToken)));
	tokenFetcher->fetchToken();
}

void OAuthAuthorizationChain::accessTokenFetched(OAuthToken token)
{
	AccessToken = token;
	emit authorized(AccessToken);

	deleteLater();
}

#include "moc_oauth-authorization-chain.cpp"
