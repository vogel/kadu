/*
 * pepmanager.cpp - Classes for PEP
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
 
#include "pep-manager.h"

#include <QtDebug>
#include "xmpp_xmlcommon.h"
#include "xmpp_tasks.h"
#include "server-info-manager.h"

// TODO: Get affiliations upon startup, and only create nodes based on that.
// (subscriptions is not accurate, since one doesn't subscribe to the
// avatar data node)

// -----------------------------------------------------------------------------

class PEPGetTask : public XMPP::Task
{
public:
	PEPGetTask(Task* parent, const QString& jid, const QString& node, const QString& itemID) : Task(parent), jid_(jid), node_(node) {
		iq_ = createIQ(doc(), "get", jid_, id());
		
		QDomElement pubsub = doc()->createElement("pubsub");
		pubsub.setAttribute("xmlns", "http://jabber.org/protocol/pubsub");
		iq_.appendChild(pubsub);
		
		QDomElement items = doc()->createElement("items");
		items.setAttribute("node", node);
		pubsub.appendChild(items);
		
		QDomElement item = doc()->createElement("item");
		item.setAttribute("id", itemID);
		items.appendChild(item);
	}
	
	void onGo() {
		send(iq_);
	}
	
	bool take(const QDomElement &x) {
		if(!iqVerify(x, jid_, id()))
			return false;

		if(x.attribute("type") == "result") {
			bool found;
			// FIXME Check namespace...
			QDomElement e = findSubTag(x, "pubsub", &found);
			if (found) {
				QDomElement i = findSubTag(e, "items", &found);
				if (found) {
					for(QDomNode n1 = i.firstChild(); !n1.isNull(); n1 = n1.nextSibling()) {
						QDomElement e1 = n1.toElement();
						if (!e1.isNull() && e1.tagName() == "item") {
							for(QDomNode n2 = e1.firstChild(); !n2.isNull(); n2 = n2.nextSibling()) {
								QDomElement e2 = n2.toElement();
								if (!e2.isNull()) {
									items_ += XMPP::PubSubItem(e1.attribute("id"),e2);
								}
							}
						}
					}
				}
			}
			setSuccess();
			return true;
		}
		else {
			setError(x);
			return true;
		}
	}
		
	const QList<XMPP::PubSubItem>& items() const {
		return items_;
	}

	const QString& jid() const {
		return jid_;
	}
	
	const QString& node() const {
		return node_;
	}

private:
	QDomElement iq_;
	QString jid_;
	QString node_;
	QList<XMPP::PubSubItem> items_;
};

class PEPPublishTask : public XMPP::Task
{
public:
	PEPPublishTask(Task* parent, const QString& node, const XMPP::PubSubItem& it, PEPManager::Access access) : XMPP::Task(parent), node_(node), item_(it) {
		iq_ = createIQ(doc(), "set", QString(), id());
		
		QDomElement pubsub = doc()->createElement("pubsub");
		pubsub.setAttribute("xmlns", "http://jabber.org/protocol/pubsub");
		iq_.appendChild(pubsub);
		
		QDomElement publish = doc()->createElement("publish");
		publish.setAttribute("node", node);
		pubsub.appendChild(publish);
		
		QDomElement item = doc()->createElement("item");
		item.setAttribute("id", it.id());
		publish.appendChild(item);

		if (access != PEPManager::DefaultAccess) {
			QDomElement conf = doc()->createElement("configure");
			QDomElement conf_x = doc()->createElementNS("jabber:x:data","x");

			// Form type
			QDomElement conf_x_field_type = doc()->createElement("field");
			conf_x_field_type.setAttribute("var","FORM_TYPE");
			conf_x_field_type.setAttribute("type","hidden");
			QDomElement conf_x_field_type_value = doc()->createElement("value");
			conf_x_field_type_value.appendChild(doc()->createTextNode("http://jabber.org/protocol/pubsub#node_config"));
			conf_x_field_type.appendChild(conf_x_field_type_value);
			conf_x.appendChild(conf_x_field_type);
			
			// Access model
			QDomElement access_model = doc()->createElement("field");
			access_model.setAttribute("var","pubsub#access_model");
			QDomElement access_model_value = doc()->createElement("value");
			access_model.appendChild(access_model_value);
			if (access == PEPManager::PublicAccess) {
				access_model_value.appendChild(doc()->createTextNode("open"));
			}
			else if (access == PEPManager::PresenceAccess) {
				access_model_value.appendChild(doc()->createTextNode("presence"));
			}
			conf_x.appendChild(access_model);
			
			
			conf.appendChild(conf_x);
			pubsub.appendChild(conf);
		}
		
		item.appendChild(it.payload());
	}
	
	bool take(const QDomElement& x) {
		if(!iqVerify(x, QString(), id()))
			return false;

		if(x.attribute("type") == "result") {
			setSuccess();
		}
		else {
			setError(x);
		}
		return true;
	}
	
	void onGo() {
		send(iq_);
	}

	const XMPP::PubSubItem& item() const {
		return item_;
	}

	const QString& node() const {
		return node_;
	}

private:
	QDomElement iq_;
	QString node_;
	XMPP::PubSubItem item_;
};


// -----------------------------------------------------------------------------

class PEPRetractTask : public XMPP::Task
{
public:
	PEPRetractTask(Task* parent, const QString& node, const QString& itemId) : XMPP::Task(parent), node_(node), itemId_(itemId) {
		iq_ = createIQ(doc(), "set", QString(), id());
		
		QDomElement pubsub = doc()->createElement("pubsub");
		pubsub.setAttribute("xmlns", "http://jabber.org/protocol/pubsub");
		iq_.appendChild(pubsub);
		
		QDomElement retract = doc()->createElement("retract");
		retract.setAttribute("node", node);
		retract.setAttribute("notify", "1");
		pubsub.appendChild(retract);
		
		QDomElement item = doc()->createElement("item");
		item.setAttribute("id", itemId);
		retract.appendChild(item);
	}
	
	bool take(const QDomElement& x) {
		if(!iqVerify(x, QString(), id()))
			return false;

		if(x.attribute("type") == "result") {
			setSuccess();
		}
		else {
			setError(x);
		}
		return true;
	}
	
	void onGo() {
		send(iq_);
	}

	const QString& node() const {
		return node_;
	}

private:
	QDomElement iq_;
	QString node_;
	QString itemId_;
};

PEPManager::PEPManager(XMPP::Client *client, ServerInfoManager *serverInfo, QObject *parent) :
		QObject(parent), client_(client), serverInfo_(serverInfo)
{
	connect(client_, SIGNAL(messageReceived(const Message &)), SLOT(messageReceived(const Message &)));
}

void PEPManager::publish(const QString& node, const XMPP::PubSubItem& it, Access access)
{
	if (!serverInfo_->hasPEP())
		return;
	
	PEPPublishTask* tp = new PEPPublishTask(client_->rootTask(),node,it,access);
	connect(tp, SIGNAL(finished()), SLOT(publishFinished()));
	tp->go(true);
}


void PEPManager::retract(const QString& node, const QString& id)
{
	if (!serverInfo_->hasPEP())
		return;
	
	PEPRetractTask* tp = new PEPRetractTask(client_->rootTask(),node,id);
	// FIXME: add notification of success/failure
	tp->go(true);
}


void PEPManager::publishFinished()
{
	PEPPublishTask* task = (PEPPublishTask*) sender();
	if (task->success()) {
		emit publish_success(task->node(),task->item());
	}
	else {
		qWarning() << QString("[%3] PEP Publish failed: '%1' (%2)").arg(task->statusString()).arg(QString::number(task->statusCode())).arg(client_->jid().full());
		emit publish_error(task->node(),task->item());
	}
}

void PEPManager::get(const XMPP::Jid& jid, const QString& node, const QString& id) 
{
	PEPGetTask* g = new PEPGetTask(client_->rootTask(),jid.bare(),node,id);
	connect(g, SIGNAL(finished()), SLOT(getFinished()));
	g->go(true);
}

void PEPManager::messageReceived(const Message &m)
{
	foreach (const XMPP::PubSubRetraction &i, m.pubsubRetractions()) {
		emit itemRetracted(m.from(),m.pubsubNode(), i);
	}
	foreach (const XMPP::PubSubItem &i, m.pubsubItems()) {
		emit itemPublished(m.from(),m.pubsubNode(),i);
	}
}

void PEPManager::getFinished()
{
	PEPGetTask* task = (PEPGetTask*) sender();
	if (task->success()) {
		// Act as if the item was published. This is a convenience 
		// implementation, probably should be changed later.
		if (!task->items().isEmpty()) {
			emit itemPublished(task->jid(),task->node(),task->items().first());
		}
	}
	else {
		qWarning() << QString("[%3] PEP Get failed: '%1' (%2)").arg(task->statusString()).arg(QString::number(task->statusCode())).arg(client_->jid().full());
	}
}
