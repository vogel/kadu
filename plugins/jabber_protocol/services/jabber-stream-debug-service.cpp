/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006 Remko Troncon
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

#include <xmpp_client.h>

#include "jabber-protocol.h"

#include "jabber-stream-debug-service.h"

namespace XMPP
{

JabberStreamDebugService::JabberStreamDebugService(JabberProtocol *protocol) :
		QObject(protocol), XmppClient(protocol->xmppClient())
{
	connect(XmppClient.data(), SIGNAL(xmlIncoming(QString)), this, SLOT(incomingXml(QString)));
	connect(XmppClient.data(), SIGNAL(xmlOutgoing(QString)), this, SLOT(outgoingXml(QString)));
}

JabberStreamDebugService::~JabberStreamDebugService()
{
}

QString JabberStreamDebugService::filterPrivateData(const QString &streamData)
{
	QString result = streamData;
	return result
			.replace(QRegExp("<password>[^<]*</password>\n"), "<password>[Filtered]</password>\n")
			.replace(QRegExp("<digest>[^<]*</digest>\n"), "<digest>[Filtered]</digest>\n");
}

void JabberStreamDebugService::incomingXml(const QString &xmlData)
{
	emit incomingStream(filterPrivateData(xmlData));
}

void JabberStreamDebugService::outgoingXml(const QString &xmlData)
{
	emit outgoingStream(filterPrivateData(xmlData));
}

}

#include "moc_jabber-stream-debug-service.cpp"
