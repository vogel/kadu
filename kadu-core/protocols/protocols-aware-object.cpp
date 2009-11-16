/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocols-manager.h"

#include "protocols-aware-object.h"

KADU_AWARE_CLASS(ProtocolsAwareObject)

void ProtocolsAwareObject::notifyProtocolRegistered(ProtocolFactory *protocol)
{
	foreach (ProtocolsAwareObject *object, Objects)
		object->protocolRegistered(protocol);
}

void ProtocolsAwareObject::notifyProtocolUnregistered(ProtocolFactory *protocol)
{
	foreach (ProtocolsAwareObject *object, Objects)
		object->protocolUnregistered(protocol);
}

void ProtocolsAwareObject::triggerAllProtocolsRegistered()
{
	foreach (ProtocolFactory *protocol, ProtocolsManager::instance()->protocolFactories())
		protocolRegistered(protocol);
}

void ProtocolsAwareObject::triggerAllProtocolsUnregistered()
{
	foreach (ProtocolFactory *protocol, ProtocolsManager::instance()->protocolFactories())
		protocolUnregistered(protocol);
}
