/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol-factory.h"

#include "debug.h"

#include "protocols-manager.h"

ProtocolsManager * ProtocolsManager::Instance = 0;

ProtocolsManager * ProtocolsManager::instance()
{
	if (0 == Instance)
		Instance = new ProtocolsManager();

	return Instance;
}

ProtocolsManager::ProtocolsManager()
{
	ConnectionErrorNotification::registerEvent();
}

ProtocolsManager::~ProtocolsManager()
{
	ConnectionErrorNotification::unregisterEvent();
}

void ProtocolsManager::registerProtocolFactory(ProtocolFactory *factory)
{
	if (!factory || Factories.contains(factory))
		return;

	emit protocolFactoryAboutToBeRegistered(factory);
	Factories.append(factory);
	emit protocolFactoryRegistered(factory);
}

void ProtocolsManager::unregisterProtocolFactory(ProtocolFactory *factory)
{
	if (!factory || !Factories.contains(factory))
		return;

	emit protocolFactoryAboutToBeUnregistered(factory);
	Factories.removeAll(factory);
	emit protocolFactoryUnregistered(factory);
}

bool ProtocolsManager::hasProtocolFactory(const QString &name)
{
	foreach (ProtocolFactory *factory, Factories)
		if (factory->name() == name)
			return true;

	return false;
}

ProtocolFactory * ProtocolsManager::byName(const QString &name)
{
	foreach (ProtocolFactory *factory, Factories)
		if (factory->name() == name)
			return factory;

	return 0;
}

ProtocolFactory * ProtocolsManager::byIndex(int index)
{
	if (index < 0 || index >= Factories.count())
		return 0;

	return Factories[index];
}
