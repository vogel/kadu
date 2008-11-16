/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "gadu.h"

#include "userlist-private.h"

ProtocolData::ProtocolData(const QString &/*protocolName*/, const QString &id)
	: ID(id), Stat(new SharedStatus(new GaduStatus()))
{
	//Stat = ProtocolManager::newStatus(protocolName);
}

ProtocolData::ProtocolData()
	: ID(), Stat(new SharedStatus(new GaduStatus()))
{
}

ProtocolData::~ProtocolData()
{
}

ProtocolData::ProtocolData(const ProtocolData &s)
	: ID(s.ID), Stat(s.Stat), data(s.data)
{
	kdebugf();
}

ProtocolData & ProtocolData::operator = (const ProtocolData &s)
{
	kdebugf();
	ID = s.ID;

 	Stat = s.Stat;
	data = s.data;

	return *this;
}

static QString DNSName("DNSName");

void ULEPrivate::setDNSName(const QString &protocolName, const QString &dnsname)
{
	// simplified implementation of setProtocolData()
	if (!protocols.contains(protocolName))
		return;

	emit dnsNameResolved(protocolName, dnsname);
}

void ULEPrivate::closeModule()
{
#ifdef DEBUG_ENABLED
	// do it only for valgrind
	userDataProxy.clear();
	statusChangeProxy.clear();
	protocolUserDataProxy.clear();
#endif
}

QHash<QString, QHash<QString, UserGroupSet> > ULEPrivate::protocolUserDataProxy;
QHash<QString, UserGroupSet> ULEPrivate::userDataProxy;
//QDict<QPtrDict<void> > ULEPrivate::addProtocolProxy;
//QDict<QPtrDict<void> > ULEPrivate::removeProtocolProxy;
QHash<QString, UserGroupSet> ULEPrivate::statusChangeProxy;
