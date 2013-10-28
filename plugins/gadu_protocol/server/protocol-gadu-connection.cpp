/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "server/gadu-writable-session-token.h"
#include "gadu-protocol.h"

#include "protocol-gadu-connection.h"

ProtocolGaduConnection::ProtocolGaduConnection(QObject *parent) :
		GaduConnection(parent)
{
}

ProtocolGaduConnection::~ProtocolGaduConnection()
{
}

void ProtocolGaduConnection::setConnectionProtocol(GaduProtocol *protocol)
{
	ConnectionProtocol = protocol;
}

bool ProtocolGaduConnection::hasSession()
{
	if (ConnectionProtocol)
		return 0 != ConnectionProtocol.data()->gaduSession();
	else
		return false;
}

gg_session * ProtocolGaduConnection::rawSession()
{
	if (ConnectionProtocol)
		return ConnectionProtocol.data()->gaduSession();
	else
		return 0;
}

bool ProtocolGaduConnection::beginWrite()
{
	if (!ConnectionProtocol)
		return false;

	ConnectionProtocol.data()->disableSocketNotifiers();
	return true;
}

bool ProtocolGaduConnection::endWrite()
{
	if (!ConnectionProtocol)
		return false;

	ConnectionProtocol.data()->enableSocketNotifiers();
	return true;
}

std::unique_ptr<GaduWritableSessionToken> ProtocolGaduConnection::writableSessionToken()
{
	return std::unique_ptr<GaduWritableSessionToken>(new GaduWritableSessionToken(this));
}

#include "moc_protocol-gadu-connection.cpp"
