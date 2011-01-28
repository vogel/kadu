/*
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef PEP_MANAGER_H
#define PEP_MANAGER_H

#include <QObject>

#include "iris/xmpp_message.h"

class ServerInfoManager;

namespace XMPP
{
	class Client;
}

class PEPManager : public QObject
{
	Q_OBJECT

	// a workaround to Qt's MOC not doing really well when mixing namespaces
	typedef XMPP::Message Message;

	XMPP::Client *client_;
	ServerInfoManager *serverInfo_;

protected slots:
	void messageReceived(const Message &m);
	void getFinished();
	void publishFinished();

public:
	enum Access {
		DefaultAccess,
		PresenceAccess,
		PublicAccess
	};

	PEPManager(XMPP::Client *client, ServerInfoManager *serverInfo, QObject *parent = 0);
	virtual ~PEPManager();

	void publish(const QString &node, const XMPP::PubSubItem &, Access = DefaultAccess);
	void retract(const QString &node, const QString &id);
	void get(const XMPP::Jid &jid, const QString &node, const QString &id);

signals:
	void publish_success(const QString &, const XMPP::PubSubItem &);
	void publish_error(const QString &, const XMPP::PubSubItem &);
	void itemPublished(const XMPP::Jid &jid, const QString & node, const XMPP::PubSubItem &);
	void itemRetracted(const XMPP::Jid &jid, const QString & node, const XMPP::PubSubRetraction &);

};

#endif // PEP_MANAGER_H
