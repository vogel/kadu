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

ULEPrivate::ULEPrivate()
	: key(0)
{
}

ULEPrivate::~ULEPrivate()
{
}

ProtocolData::ProtocolData(const QString &/*protocolName*/, const QString &id)
	: ID(id), Stat(new GaduStatus())
{
	//Stat = ProtocolManager::newStatus(protocolName);
}

ProtocolData::ProtocolData()
	: ID(), Stat(new UserStatus())
{
}

ProtocolData::~ProtocolData()
{
	delete Stat;
}

ProtocolData::ProtocolData(const ProtocolData &s)
	: ID(s.ID), Stat(s.Stat->copy()), data(s.data)
{
	kdebugf();
}

ProtocolData & ProtocolData::operator = (const ProtocolData &s)
{
	kdebugf();
	ID = s.ID;
	delete Stat;

 	Stat = s.Stat->copy();
	data = s.data;

	return *this;
}

static QString DNSName("DNSName");

void ULEPrivate::setDNSName(const QString &protocolName, const QString &dnsname)
{
	// simplified implementation of setProtocolData()
	ProtocolData *protocolData = *protocols.find(protocolName);
	if (!protocolData)
		return;
	QVariant *old = protocolData->data[DNSName];
	if (old)
	{
		if (dnsname == old->toString())
			return;
		protocolData->data[DNSName] = new QVariant(dnsname);
	}
	else
	{
		protocolData->data.insert(DNSName, new QVariant(dnsname));
		old = new QVariant();
	}

// TODO: 0.6.5
// 	foreach(UserGroup *group, Parents)
// 		emit group->protocolUserDataChanged(protocolName, userlist->byKey(key), DNSName, *old, dnsname, false, false);

	delete old;
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
