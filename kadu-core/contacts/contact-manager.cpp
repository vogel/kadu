/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "debug.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
		Instance = new ContactManager();

	return Instance;
}

ContactManager::ContactManager()
{
}

ContactManager::~ContactManager()
{
}

void ContactManager::itemAdded(Contact item)
{
	connect(item, SIGNAL(protocolLoaded()), this, SLOT(contactProtocolLoaded()));
	connect(item, SIGNAL(protocolUnloaded()), this, SLOT(contactProtocolUnloaded()));
}

void ContactManager::itemRemoved(Contact item)
{
	disconnect(item, SIGNAL(protocolLoaded()), this, SLOT(contactProtocolLoaded()));
	disconnect(item, SIGNAL(protocolUnloaded()), this, SLOT(contactProtocolUnloaded()));
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
}

void ContactManager::itemRegisterd(Contact item)
{
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
}

void ContactManager::itemUnregistered(Contact item)
{
}

void ContactManager::contactProtocolLoaded()
{
	Contact contact(sender());
	if (!contact.isNull())
		registerItem(contact);
}

void ContactManager::contactProtocolUnloaded()
{
	Contact contact(sender());
	if (!contact.isNull())
		unregisterItem(contact);
}
