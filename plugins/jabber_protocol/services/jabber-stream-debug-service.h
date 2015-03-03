/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <qxmpp/QXmppLogger.h>

class QXmppClient;

class JabberStreamDebugService : public QObject
{
	Q_OBJECT

public:
	explicit JabberStreamDebugService(QXmppClient *m_client, QObject *parent = nullptr);
	virtual ~JabberStreamDebugService();

signals:
	void incomingStream(const QString &streamData);
	void outgoingStream(const QString &streamData);

private:
	QString filterPrivateData(const QString &streamData);

private slots:
	void message(QXmppLogger::MessageType type, const QString &message);

};
