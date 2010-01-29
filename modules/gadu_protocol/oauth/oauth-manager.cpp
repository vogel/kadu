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

#include "oauth/oauth-authorization.h"
#include "oauth/oauth-token-fetcher.h"

#include "oauth-manager.h"

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

	OAuthAuthorization *authorization = new OAuthAuthorization("http://login.gadu-gadu.pl/authorize", "http://www.mojageneracja.pl",
			token.consumer(), NetworkManager, this);
	authorization->authorize(token);
}
