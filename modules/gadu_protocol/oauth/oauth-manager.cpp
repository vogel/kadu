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

#include "oauth/oauth-authorization.h"
#include "oauth/oauth-authorization-chain.h"
#include "oauth/oauth-parameters.h"
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

void OAuthManager::authorize(OAuthConsumer consumer)
{
	OAuthAuthorizationChain *chain = new OAuthAuthorizationChain(consumer, NetworkManager, this);
	chain->setRequestTokenUrl("http://api.gadu-gadu.pl/request_token");
	chain->setAuthorizeUrl("https://login.gadu-gadu.pl/authorize");
	chain->setAuthorizeCallbackUrl("http://www.mojageneracja.pl");
	chain->setAccessTokenUrl("http://api.gadu-gadu.pl/access_token");

	connect(chain, SIGNAL(authorized(OAuthToken)), this, SIGNAL(authorized(OAuthToken)));
	chain->authorize();
}
