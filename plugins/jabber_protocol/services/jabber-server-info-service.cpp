/*
 * serverinfomanager.cpp
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "xmpp_tasks.h"

#include "jabber-protocol.h"

#include "jabber-server-info-service.h"

namespace XMPP
{

JabberServerInfoService::JabberServerInfoService(JabberProtocol *protocol) :
		QObject(protocol), XmppClient(protocol->xmppClient()), SupportsPep(false)
{
	connect(XmppClient.data(), SIGNAL(disconnected()), SLOT(reset()));
}

JabberServerInfoService::~JabberServerInfoService()
{
}

const Features & JabberServerInfoService::features() const
{
	return ServerFeatures;
}

const DiscoItem::Identities & JabberServerInfoService::identities() const
{
	return ServerIdentities;
}

bool JabberServerInfoService::supportsPep() const
{
	return SupportsPep;
}

void JabberServerInfoService::requestServerInfo()
{
	if (!XmppClient)
		return;

	XMPP::JT_DiscoInfo *jt = new XMPP::JT_DiscoInfo(XmppClient->rootTask());
	connect(jt, SIGNAL(finished()), SLOT(requestFinished()));
	jt->get(XmppClient->jid().domain());
	jt->go(true);
}

void JabberServerInfoService::requestFinished()
{
	XMPP::JT_DiscoInfo *jt = qobject_cast<XMPP::JT_DiscoInfo *>(sender());
	if (!jt || !jt->success())
		return;

	ServerFeatures = jt->item().features();
	ServerIdentities = jt->item().identities();

	// TODO: Remove this, this is legacy
	if (ServerFeatures.test(QStringList("http://jabber.org/protocol/pubsub#pep")))
		SupportsPep = true;

	foreach(const XMPP::DiscoItem::Identity &identity, ServerIdentities)
		if (identity.category == "pubsub" && identity.type == "pep")
			SupportsPep = true;

	emit updated();
}

void JabberServerInfoService::reset()
{
	SupportsPep = false;
	emit updated();
}

}

#include "moc_jabber-server-info-service.cpp"
