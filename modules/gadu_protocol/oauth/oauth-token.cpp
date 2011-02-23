/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "oauth-token.h"

OAuthToken::OAuthToken() :
		Valid(false), TokenExpiresIn(0)
{
}

OAuthToken::OAuthToken(const OAuthToken &copyMe)
{
	Consumer = copyMe.Consumer;
	Valid = copyMe.Valid;
	Token = copyMe.Token;
	TokenSecret = copyMe.TokenSecret;
	TokenExpiresIn = copyMe.TokenExpiresIn;
}

OAuthToken::OAuthToken(const QByteArray &token, const QByteArray &tokenSecret, int tokenExpiresIn) :
		Valid(true), Token(token), TokenSecret(tokenSecret), TokenExpiresIn(tokenExpiresIn)
{
}

OAuthToken & OAuthToken::operator = (const OAuthToken &copyMe)
{
	Consumer = copyMe.Consumer;
	Valid = copyMe.Valid;
	Token = copyMe.Token;
	TokenSecret = copyMe.TokenSecret;
	TokenExpiresIn = copyMe.TokenExpiresIn;

	return *this;
}

bool OAuthToken::isValid() const
{
	return Valid;
}

const QByteArray & OAuthToken::token() const
{
	return Token;
}

const QByteArray & OAuthToken::tokenSecret() const
{
	return TokenSecret;
}

int OAuthToken::tokenExpiresIn() const
{
	return TokenExpiresIn;
}

void OAuthToken::setConsumer(const OAuthConsumer &consumer)
{
	Consumer = consumer;
}

const OAuthConsumer & OAuthToken::consumer() const
{
	return Consumer;
}
