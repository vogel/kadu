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
#define KADU_USERLIST_PRIVATE
#include "userlist-private.h"

// UWAGA: wraz ze wzrostem liczby mo¿liwych kluczy nale¿y powiêkszaæ
// domy¶lne rozmiary s³owników na liczby pierwsze!
// mo¿na je braæ st±d: http://doc.trolltech.com/3.3/primes.html
ULEPrivate::ULEPrivate() : informations(31), protocols(3)
{
}

ULEPrivate::~ULEPrivate()
{
	informations.setAutoDelete(true);
	protocols.setAutoDelete(true);
}

ProtocolData::ProtocolData(const QString &protocolName, const QString &id) : data(31)
{
	ID = id;
	Stat = new GaduStatus();
	//Stat = ProtocolManager::newStatus(protocolName);
}

ProtocolData::ProtocolData() : Stat(new UserStatus()), data(31)
{
}

ProtocolData::~ProtocolData()
{
	delete Stat;
	data.setAutoDelete(true);
}

ProtocolData::ProtocolData(const ProtocolData &s)
{
	kdebugf();
	ID = s.ID;
	delete Stat;
 	Stat = s.Stat->copy();
	data = s.data;
}

void ProtocolData::operator=(const ProtocolData &s)
{
	kdebugf();
	ID = s.ID;
	delete Stat;
 	Stat = s.Stat->copy();
	data = s.data;
}

void ULEPrivate::setDNSName(const QString &protocolName, const QString &dnsname)
{
	//uproszczona implementacja setProtocolData()
	ProtocolData *protocolData = protocols.find(protocolName);
	if (protocolData == NULL)
		return;
	QVariant *old = protocolData->data["DNSName"];
	if (old && dnsname == old->toString())
		return;
	if (old)
		protocolData->data.replace("DNSName", new QVariant(dnsname));
	else
		protocolData->data.insert("DNSName", new QVariant(dnsname));
	if (!old)
		old = new QVariant();
	CONST_FOREACH (group, Parents)
		emit (*group)->protocolUserDataChanged(protocolName, userlist->byKey(key), "DNSName", *old, dnsname, false, false);
	delete old;
}

UserGroupData::UserGroupData(int size) : data(size)
{
}

UserGroupData::~UserGroupData()
{
	data.setAutoDelete(true);
}

