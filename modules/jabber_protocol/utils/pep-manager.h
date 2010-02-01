/*
 * pepmanager.h - Classes for PEP
 * Copyright (C) 2006  Remko Troncon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef PEPMANAGER_H
#define PEPMANAGER_H

#include <QObject>

#include "xmpp_tasks.h"

class PubSubSubscription;
class QString;
class ServerInfoManager;

class PEPManager : public QObject
{
	Q_OBJECT
	
public:
	enum Access {
		DefaultAccess,
		PresenceAccess,
		PublicAccess
	};

	PEPManager(XMPP::Client* client, ServerInfoManager* serverInfo);

	void publish(const QString& node, const XMPP::PubSubItem&, Access = DefaultAccess);
	void retract(const QString& node, const QString& id);
	void get(const XMPP::Jid& jid, const QString& node, const QString& id);

signals:
	void publish_success(const QString&, const XMPP::PubSubItem&);
	void publish_error(const QString&, const XMPP::PubSubItem&);
	void itemPublished(const XMPP::Jid& jid, const QString& node, const XMPP::PubSubItem&);
	void itemRetracted(const XMPP::Jid& jid, const QString& node, const XMPP::PubSubRetraction&);

protected slots:
	void messageReceived(const XMPP::Message&);
	void getFinished();
	void publishFinished();

private:
	XMPP::Client* client_;
	ServerInfoManager* serverInfo_;
};

#endif
