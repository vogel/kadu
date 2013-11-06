/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "server/gadu-connection.h"

#include "gadu-writable-session-token.h"

GaduWritableSessionToken::GaduWritableSessionToken(GaduConnection *connection) :
		Connection(connection)
{
	Connection->beginWrite();
}

GaduWritableSessionToken::GaduWritableSessionToken(GaduWritableSessionToken &&moveMe)
{
	Connection = moveMe.Connection;
	moveMe.Connection = nullptr;
}

GaduWritableSessionToken::~GaduWritableSessionToken()
{
	if (Connection)
		Connection->endWrite();
}

GaduWritableSessionToken & GaduWritableSessionToken::operator = (GaduWritableSessionToken &&moveMe)
{
	Connection = moveMe.Connection;
	moveMe.Connection = nullptr;

	return *this;
}

gg_session * GaduWritableSessionToken::rawSession() const
{
	return Connection ? Connection->rawSession() : nullptr;
}
