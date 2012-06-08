/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-protocol.h"

#include "jabber-client-info-service.h"

using namespace XMPP;

JabberClientInfoService::JabberClientInfoService(JabberProtocol *protocol) :
		QObject(protocol), XmppClient(protocol->xmppClient())
{
}

JabberClientInfoService::~JabberClientInfoService()
{
}

void JabberClientInfoService::setClientName(const QString &clientName)
{
	ClientName = clientName;
}

void JabberClientInfoService::setClientVersion(const QString &clientVersion)
{
	ClientVersion = clientVersion;
}

void JabberClientInfoService::setOSName(const QString &osName)
{
	OSName = osName;
}

void JabberClientInfoService::sendClientInfo()
{
	if (!XmppClient)
		return;

	XmppClient.data()->setClientName(ClientName);
	XmppClient.data()->setClientVersion(ClientVersion);
	XmppClient.data()->setOSName(OSName);
}
