/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef JABBER_STREAM_DEBUG_SERVICE_H
#define JABBER_STREAM_DEBUG_SERVICE_H

#include <QtCore/QPointer>

namespace XMPP
{

class Client;

class JabberProtocol;

class JabberStreamDebugService : public QObject
{
	Q_OBJECT

	QPointer<XMPP::Client> XmppClient;

	QString filterPrivateData(const QString &streamData);

private slots:
	void incomingXml(const QString &xmlData);
	void outgoingXml(const QString &xmlData);

public:
	explicit JabberStreamDebugService(JabberProtocol *protocol);
	virtual ~JabberStreamDebugService();

signals:
	void incomingStream(const QString &streamData);
	void outgoingStream(const QString &streamData);

};

}

#endif // JABBER_STREAM_DEBUG_SERVICE_H
