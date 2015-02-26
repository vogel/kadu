/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-resource.h"

JabberResource::JabberResource() :
		_priority{-1}
{
}

JabberResource::JabberResource(Jid jid, int priority, Status status) :
		_jid{std::move(jid)},
		_priority{priority},
		_status{std::move(status)}
{
}

bool JabberResource::isEmpty() const
{
	return _jid.isEmpty();
}

Jid JabberResource::jid() const
{
	return _jid;
}

int JabberResource::priority() const
{
	return _priority;
}

Status JabberResource::status() const
{
	return _status;
}
