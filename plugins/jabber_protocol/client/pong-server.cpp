/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Based on pongserver.cpp file from PSI
 * Copyright (C) 2007  Maciej Niedzielski
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

#include "xmpp_xmlcommon.h"

#include "pong-server.h"

PongServer::PongServer(Task *parentTask) :
		Task(parentTask)
{
}

PongServer::~PongServer()
{
}

bool PongServer::take(const QDomElement &element)
{
	if (element.tagName() != "iq" || element.attribute("type") != "get")
		return false;

	bool found = false;
	QDomElement ping = findSubTag(element, "ping", &found);

	if (found && ping.attribute("xmlns") == "urn:xmpp:ping")
	{
		QDomElement iq = createIQ(doc(), "result", element.attribute("from"), element.attribute("id"));
		send(iq);
		return true;
	}

	return false;
}
