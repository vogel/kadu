/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "actions/jabber-actions.h"
#include "gui/menu/menu-inventory.h"

#include "jabber-protocol-menu-manager.h"

JabberProtocolMenuManager * JabberProtocolMenuManager::Instance = 0;

void JabberProtocolMenuManager::createInstance()
{
	if (!Instance)
		Instance = new JabberProtocolMenuManager();
}

void JabberProtocolMenuManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

JabberProtocolMenuManager::JabberProtocolMenuManager()
{
	RosterActions.append(JabberActions::instance()->resendSubscription());
	RosterActions.append(JabberActions::instance()->removeSubscription());
	RosterActions.append(JabberActions::instance()->askForSubscription());

	MenuInventory::instance()->registerProtocolMenuManager(this);
}

JabberProtocolMenuManager::~JabberProtocolMenuManager()
{
	RosterActions.clear();

	MenuInventory::instance()->unregisterProtocolMenuManager(this);
}

const QList<ActionDescription *> & JabberProtocolMenuManager::protocolActions() const
{
	return RosterActions;
}
