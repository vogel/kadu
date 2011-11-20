/*
 * Copyright (C) 2006  Remko Troncon
 *
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "pep-retract-task.h"

PEPRetractTask::PEPRetractTask(Task *parent, const QString &node, const QString &itemId) :
		XMPP::Task(parent), node_(node), itemId_(itemId)
{
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

PEPRetractTask::~PEPRetractTask()
{
}

bool PEPRetractTask::take(const QDomElement &x)
{
	if (!iqVerify(x, QString(), id()))
		return false;

	if (x.attribute("type") == "result")
		setSuccess();
	else
		setError(x);

	return true;
}

void PEPRetractTask::onGo()
{
	send(iq_);
}

const QString & PEPRetractTask::node() const
{
	return node_;
}
