/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "misc/misc.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol_factory.h"

#include "debug.h"
#include "xml_config_file.h"

#include "protocols_manager.h"

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

void ProtocolsManager::registerProtocolFactory(const QString &name, ProtocolFactory *factory)
{
	if (0 != factory && !registeredFactories.contains(name))
	{
		registeredFactories[name] = factory;
		AccountManager::instance()->loadConfiguration(name);
	}
}

void ProtocolsManager::unregisterProtocolFactory(const QString &name)
{
	if (registeredFactories.contains(name))
	{
		AccountManager::instance()->storeConfiguration(name);
		delete registeredFactories[name];
		registeredFactories.remove(name);
	}
}

bool ProtocolsManager::hasProtocolFactory(const QString& name)
{
	return registeredFactories.contains(name);
}

ProtocolFactory * ProtocolsManager::protocolFactory(const QString &name)
{
	return registeredFactories.contains(name)
		? registeredFactories[name]
		: 0;
}
