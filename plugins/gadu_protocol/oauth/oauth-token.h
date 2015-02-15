/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OAUTH_TOKEN_H
#define OAUTH_TOKEN_H

#include <QtCore/QString>

#include "oauth/oauth-consumer.h"

class OAuthToken
{
	OAuthConsumer Consumer;

	bool Valid;
	QByteArray Token;
	QByteArray TokenSecret;
	int TokenExpiresIn;

public:
	OAuthToken();
	OAuthToken(const OAuthToken &copyMe);
	OAuthToken(const QByteArray &token, const QByteArray &tokenSecret, int tokenExpiresIn);

	OAuthToken & operator = (const OAuthToken &copyMe);

	bool isValid() const;
	const QByteArray & token() const;
	const QByteArray & tokenSecret() const;
	int tokenExpiresIn() const;

	void setConsumer(const OAuthConsumer &consumer);
	const OAuthConsumer & consumer() const;

};

#endif // OAUTH_TOKEN_H
