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

ProtocolsManager::ProtocolsManager()
	: QObject(), protocolDescriptions(), protocols()
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
	protocols_manager = 0;
}

void ProtocolsManager::registerProtocol(const QString &protocolID, const QString &name, ProtocolManager *manager)
{
	protocolDescriptions.append(ProtocolDescription(protocolID, name, manager));
}

void ProtocolsManager::unregisterProtocol(const QString &protocolID)
{
	foreach(const ProtocolDescription &protoDesc, protocolDescriptions)
		if (protoDesc.protocolID == protocolID)
		{
			protocolDescriptions.removeAll(protoDesc);
			return;
		}

	kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", qPrintable(protocolID));
}

QList<Protocol *> ProtocolsManager::byProtocolID(const QString &protocolID)
{
	QList<Protocol *> ret;

	foreach(Protocol *proto, protocols)
		if (proto->protocolID() == protocolID)
			ret.append(proto);

	if (ret.size() == 0)
		kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", qPrintable(protocolID));

	return ret;
}

Protocol * ProtocolsManager::byID(const QString &protocolID, const QString &ID)
{
	foreach(Protocol *proto, protocols)
		if (proto->protocolID() == protocolID && proto->ID() == ID)
			return proto;

	kdebugm(KDEBUG_WARNING, "protocol,id(%s,%s) not found\n", qPrintable(protocolID), qPrintable(ID));
	return 0;
}

Protocol * ProtocolsManager::newProtocol(const QString &protocolID, const QString &ID)
{
	Protocol *proto = 0;
	foreach(const ProtocolDescription &protoDesc, protocolDescriptions)
		if (protoDesc.protocolID == protocolID)
		{
			proto = protoDesc.Manager->newInstance(ID);
			break;
		}

	if (proto)
		protocols.append(proto);
	else
		kdebugm(KDEBUG_WARNING, "protocol(%s) not found\n", qPrintable(protocolID));

	return proto;
}

ProtocolsManager *protocols_manager;
