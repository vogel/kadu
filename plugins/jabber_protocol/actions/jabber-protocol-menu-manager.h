/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef JABBER_PROTOCOL_MENU_MANAGER_H
#define JABBER_PROTOCOL_MENU_MANAGER_H

#include "protocols/protocol-menu-manager.h"

class JabberProtocolMenuManager : public ProtocolMenuManager
{
	static JabberProtocolMenuManager *Instance;

	QList<ActionDescription *> RosterActions;

	explicit JabberProtocolMenuManager();
	virtual ~JabberProtocolMenuManager();

public:
	static void createInstance();
	static void destroyInstance();

	static JabberProtocolMenuManager * instance() { return Instance; }

	virtual const QList<ActionDescription *> & protocolActions() const;

	virtual const QString protocolName() const { return "jabber"; }

};

#endif // JABBER_PROTOCOL_MENU_MANAGER_H
