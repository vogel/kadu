/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * Copyright (C) 2006  Remko Troncon
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

#ifndef JABBER_SERVER_INFO_SERVICE_H
#define JABBER_SERVER_INFO_SERVICE_H

#include <QtCore/QObject>
#include <QString>

#include "xmpp_tasks.h"

class JabberProtocol;

namespace XMPP
{

class JabberServerInfoService : public QObject
{
	Q_OBJECT

	QPointer<Client> XmppClient;
	XMPP::Features ServerFeatures;
	XMPP::DiscoItem::Identities ServerIdentities;

	bool SupportsPep;

private slots:
	void requestFinished();
	void reset();

public:
	explicit JabberServerInfoService(JabberProtocol *protocol);
	virtual ~JabberServerInfoService();

	const XMPP::Features & features() const;
	const XMPP::DiscoItem::Identities & identities() const;

	bool supportsPep() const;

	void requestServerInfo();

signals:
	void updated();

};

}

#endif // JABBER_SERVER_INFO_SERVICE_H
