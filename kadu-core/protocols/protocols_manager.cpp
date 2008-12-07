/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "misc.h"
#include "protocol_factory.h"

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
}

ProtocolsManager::~ProtocolsManager()
{
}

void ProtocolsManager::registerProtocolFactory(const QString &name, ProtocolFactory *factory)
{
	if (0 != factory && !registeredFactories.contains(name))
		registeredFactories[name] = factory;
}

void ProtocolsManager::unregisterProtocolFactory(const QString &name)
{
	if (registeredFactories.contains(name))
	{
		delete registeredFactories[name];
		registeredFactories.remove(name);
	}
}

Protocol * ProtocolsManager::newInstance(const QString &name)
{
	if (registeredFactories.contains(name))
		return registeredFactories[name]->newInstance();

	return 0;
}
