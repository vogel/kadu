/*
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "iris/xmpp_jid.h"
#include "iris/xmpp_pubsubitem.h"
#include "iris/xmpp_xmlcommon.h"

#include "pep-get-task.h"

PEPGetTask::PEPGetTask(Task *parent, const QString &jid, const QString &node, const QString &itemID) :
		Task(parent), jid_(jid), node_(node)
{
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

PEPGetTask::~PEPGetTask()
{
}

void PEPGetTask::onGo()
{
	send(iq_);
}

bool PEPGetTask::take(const QDomElement &x)
{
	if (!iqVerify(x, jid_, id()))
		return false;

	if (x.attribute("type") == "result")
	{
		// FIXME Check namespace...
		QDomElement e = x.firstChildElement("pubsub");
		if (!e.isNull()) {
			QDomElement i = e.firstChildElement("items");
			if (!i.isNull()) {
				for (QDomNode n1 = i.firstChild(); !n1.isNull(); n1 = n1.nextSibling()) {
					QDomElement e1 = n1.toElement();
					if  (!e1.isNull() && e1.tagName() == "item") {
						for (QDomNode n2 = e1.firstChild(); !n2.isNull(); n2 = n2.nextSibling()) {
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
	}
	else
		setError(x);

	return true;
}

const QList<XMPP::PubSubItem> & PEPGetTask::items() const
{
	return items_;
}

const QString & PEPGetTask::jid() const
{
	return jid_;
}

const QString & PEPGetTask::node() const
{
	return node_;
}

#include "moc_pep-get-task.cpp"
