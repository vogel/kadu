/*
 * serverinfomanager.cpp
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

#include "xmpp_tasks.h"

#include "server-info-manager.h"

ServerInfoManager::ServerInfoManager(XMPP::Client *client, QObject *parent) :
		QObject(parent), client_(client)
{
	deinitialize();
	connect(client_, SIGNAL(rosterRequestFinished(bool, int, const QString &)), SLOT(initialize()));
	connect(client_, SIGNAL(disconnected()), SLOT(deinitialize()));
}

void ServerInfoManager::reset()
{
	hasPEP_ = false;
	multicastService_.clear();
}

void ServerInfoManager::initialize()
{
	XMPP::JT_DiscoInfo *jt = new XMPP::JT_DiscoInfo(client_->rootTask());
	connect(jt, SIGNAL(finished()), SLOT(disco_finished()));
	jt->get(client_->jid().domain());
	jt->go(true);
}

void ServerInfoManager::deinitialize()
{
	reset();
	emit featuresChanged();
}

const QString& ServerInfoManager::multicastService() const
{
	return multicastService_;
}

bool ServerInfoManager::hasPEP() const
{
	return hasPEP_;
}

void ServerInfoManager::disco_finished()
{
	XMPP::JT_DiscoInfo *jt = (XMPP::JT_DiscoInfo *)sender();
	if (jt->success()) {
		// Features
		XMPP::Features f = jt->item().features();
		if (f.canMulticast())
			multicastService_ = client_->jid().domain();
		// TODO: Remove this, this is legacy
		if (f.test(QStringList("http://jabber.org/protocol/pubsub#pep")))
			hasPEP_ = true;

		// Identities
		XMPP::DiscoItem::Identities is = jt->item().identities();
		foreach(const XMPP::DiscoItem::Identity &i, is) {
			if (i.category == "pubsub" && i.type == "pep")
				hasPEP_ = true;
		}

		emit featuresChanged();
	}
}
