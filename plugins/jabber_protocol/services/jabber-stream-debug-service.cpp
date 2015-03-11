/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-stream-debug-service.h"

#include <qxmpp/QXmppClient.h>

JabberStreamDebugService::JabberStreamDebugService(QXmppClient *m_client, QObject *parent) :
		QObject{parent}
{
	m_client->setLogger(new QXmppLogger{m_client});
	m_client->logger()->setLoggingType(QXmppLogger::SignalLogging);
	connect(m_client->logger(), SIGNAL(message(QXmppLogger::MessageType,QString)), this, SLOT(message(QXmppLogger::MessageType,QString)));
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

void JabberStreamDebugService::message(QXmppLogger::MessageType type, const QString &message)
{
	if (type & QXmppLogger::MessageType::ReceivedMessage)
		emit incomingStream(filterPrivateData(message));
	else
		emit outgoingStream(filterPrivateData(message));
}

#include "moc_jabber-stream-debug-service.cpp"
