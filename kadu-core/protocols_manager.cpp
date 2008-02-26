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
#include "protocols_manager.h"
//Added by qt3to4:
#include <Q3ValueList>

ProtocolsManager::ProtocolsManager() : QObject(NULL, "protocols_manager"),
	protocolDescriptions(), protocols()
{
}

ProtocolsManager::~ProtocolsManager()
{
}

void ProtocolsManager::initModule()
{
	protocols_manager = new ProtocolsManager();
}

void ProtocolsManager::closeModule()
{
	delete protocols_manager;
	protocols_manager = NULL;
}

void ProtocolsManager::registerProtocol(const QString &protocolID, const QString &name, ProtocolManager *manager)
{
	protocolDescriptions.append(ProtocolDescription(protocolID, name, manager));
}

void ProtocolsManager::unregisterProtocol(const QString &protocolID)
{
	FOREACH(protoDesc, protocolDescriptions)
		if ((*protoDesc).protocolID == protocolID)
		{
			protocolDescriptions.erase(protoDesc);
			return;
		}
	kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", protocolID.local8Bit().data());
}

Q3ValueList<Protocol *> ProtocolsManager::byProtocolID(const QString &protocolID)
{
	Q3ValueList<Protocol *> ret;
	CONST_FOREACH(proto, protocols)
		if ((*proto)->protocolID() == protocolID)
			ret.append(*proto);
	if (ret.size() == 0)
		kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", protocolID.local8Bit().data());
	return ret;
}

Protocol *ProtocolsManager::byID(const QString &protocolID, const QString &ID)
{
	CONST_FOREACH(proto, protocols)
		if ((*proto)->protocolID() == protocolID && (*proto)->ID() == ID)
			return *proto;
	kdebugm(KDEBUG_WARNING, "protocol,id(%s,%s) not found\n", protocolID.local8Bit().data(), ID.local8Bit().data());
	return NULL;
}

Protocol *ProtocolsManager::newProtocol(const QString &protocolID, const QString &ID)
{
	Protocol *proto = NULL;
	CONST_FOREACH(protoDesc, protocolDescriptions)
		if ((*protoDesc).protocolID == protocolID)
		{
			proto = (*protoDesc).Manager->newInstance(ID);
			break;
		}
	if (proto)
		protocols.append(proto);
	else
		kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", protocolID.local8Bit().data());
	return proto;
}

ProtocolsManager *protocols_manager;
