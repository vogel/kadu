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
#include "usergroup.h"
#include "misc.h"
#include "userlist-private.h"

// WARNING: as soon as number of keys will grow, we need to increase
// default dictionary sizes to prime numbers!
// we can find them here: http://doc.trolltech.com/3.3/primes.html
ULEPrivate::ULEPrivate() : informations(31), protocols(3),
	Parents(), key(0)
{
}

ULEPrivate::~ULEPrivate()
{
	informations.setAutoDelete(true);
	protocols.setAutoDelete(true);
}

ProtocolData::ProtocolData(const QString &/*protocolName*/, const QString &id) : ID(id), Stat(new GaduStatus()), data(31)
{
	//Stat = ProtocolManager::newStatus(protocolName);
}

ProtocolData::ProtocolData() : ID(), Stat(new UserStatus()), data(31)
{
}

ProtocolData::~ProtocolData()
{
	delete Stat;
	data.setAutoDelete(true);
}

ProtocolData::ProtocolData(const ProtocolData &s) : ID(s.ID), Stat(s.Stat->copy()), data(s.data)
{
	kdebugf();
}

ProtocolData &ProtocolData::operator=(const ProtocolData &s)
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
	ProtocolData *protocolData = protocols.find(protocolName);
	if (protocolData == NULL)
		return;
	QVariant *old = protocolData->data[DNSName];
	if (old && dnsname == old->toString())
		return;
	if (old)
		protocolData->data.replace(DNSName, new QVariant(dnsname));
	else
		protocolData->data.insert(DNSName, new QVariant(dnsname));
	if (!old)
		old = new QVariant();
	CONST_FOREACH (group, Parents)
		emit (*group)->protocolUserDataChanged(protocolName, userlist->byKey(key), DNSName, *old, dnsname, false, false);
	delete old;
}

UserGroupData::UserGroupData(int size) : data(size), list()
{
}

UserGroupData::~UserGroupData()
{
	data.setAutoDelete(true);
}

void ULEPrivate::closeModule()
{
#ifdef DEBUG_ENABLED
	// do it only for valgrind
	userDataProxy.setAutoDelete(true);
	userDataProxy.clear();
	statusChangeProxy.setAutoDelete(true);
	statusChangeProxy.clear();
	protocolUserDataProxy.setAutoDelete(true);
	protocolUserDataProxy.clear();
#endif
}

QDict<QDict<UserGroupSet> > ULEPrivate::protocolUserDataProxy;
QDict<UserGroupSet> ULEPrivate::userDataProxy;
//QDict<QPtrDict<void> > ULEPrivate::addProtocolProxy;
//QDict<QPtrDict<void> > ULEPrivate::removeProtocolProxy;
QDict<UserGroupSet> ULEPrivate::statusChangeProxy;
