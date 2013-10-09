/*
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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
#include "iris/xmpp_xmlcommon.h"

#include "pep-publish-task.h"

PEPPublishTask::PEPPublishTask(Task *parent, const QString &node, const XMPP::PubSubItem &it, JabberPepService::Access access) :
		XMPP::Task(parent), node_(node), item_(it)
{
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

	if (access != JabberPepService::DefaultAccess)
	{
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
		if (access == JabberPepService::PublicAccess)
			access_model_value.appendChild(doc()->createTextNode("open"));
		else if (access == JabberPepService::PresenceAccess)
			access_model_value.appendChild(doc()->createTextNode("presence"));
		conf_x.appendChild(access_model);


		conf.appendChild(conf_x);
		pubsub.appendChild(conf);
	}

	item.appendChild(it.payload());
}

PEPPublishTask::~PEPPublishTask()
{
}

bool PEPPublishTask::take(const QDomElement &x)
{
	if (!iqVerify(x, QString(), id()))
		return false;

	if (x.attribute("type") == "result")
		setSuccess();
	else
		setError(x);

	return true;
}

void PEPPublishTask::onGo()
{
	send(iq_);
}

const XMPP::PubSubItem & PEPPublishTask::item() const
{
	return item_;
}

const QString & PEPPublishTask::node() const
{
	return node_;
}

#include "moc_pep-publish-task.cpp"
