/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "dnshandler.h"
#include "misc.h"
#include "usergroup.h"
#include "userlistelement.h"
#include "userlist-private.h"

bool UserListElement::hasIP(const QString &protocolName) const
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return false;
	}
	return protoData->data.find("IP") != NULL &&
			protocolData(protocolName, "IP").toUInt() != 0;
}

QHostAddress UserListElement::IP(const QString &protocolName) const
{
	return QHostAddress(protocolData(protocolName, "IP").toUInt());
}

QString UserListElement::DNSName(const QString &protocolName) const
{
	return protocolData(protocolName, "DNSName").toString();
}

void UserListElement::setAddressAndPort(const QString &protocolName, const QHostAddress &ip, short port)
{
	if (privateData->protocols.find(protocolName) == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return;
	}
	setDNSName(protocolName, QString::null);
	setProtocolData(protocolName, "IP", ip.toIPv4Address());
	setProtocolData(protocolName, "Port", port);
}

short UserListElement::port(const QString &protocolName) const
{
	return protocolData(protocolName, "Port").toUInt();
}

void UserListElement::refreshDNSName(const QString &protocolName)
{
	unsigned int ip = protocolData(protocolName, "IP").toUInt();
	if (ip)
		connect(new DNSHandler(protocolName, ip), SIGNAL(result(const QString &, const QString &)),
				privateData, SLOT(setDNSName(const QString &, const QString &)));
}

QVariant UserListElement::setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively, bool last)
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);

	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QVariant();
	}
	QVariant old;
	QVariant *oldVal = protoData->data[name];
	if (oldVal)
		old = *oldVal;
	if (val == old)
		return old;
	if (oldVal)
		protoData->data.replace(name, new QVariant(val));
	else
		protoData->data.insert(name, new QVariant(val));
	delete oldVal;

	CONST_FOREACH (group, privateData->Parents)
		emit (*group)->protocolUserDataChanged(protocolName, *this, name, old, val, massively, last);
	return old;
}

QVariant UserListElement::protocolData(const QString &protocolName, const QString &name) const
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QVariant();
	}
	QVariant *val = protoData->data.find(name);
	if (val)
		return *val;
	else
		return QVariant();
}

const UserStatus &UserListElement::status(const QString &protocolName) const
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		static UserStatus us;
		return us;
	}
	return *(protoData->Stat);
}

QString UserListElement::ID(const QString &protocolName) const
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QString::null;
	}
	return protoData->ID;
}

UserListElement::UserListElement(const UserListElement &copyMe) : QObject(NULL, 0)
{
//	kdebugf();
	copyMe.privateData->ref();
	privateData = copyMe.privateData;
//	kdebugf2();
}

UserListElement::UserListElement() : QObject(NULL, 0)
{
//	kdebugf();
	privateData = new ULEPrivate();
	privateData->key = used ++;
//	kdebugf2();
}

UserListElement::~UserListElement()
{
//	kdebugf();
	if (privateData->deref())
		delete privateData;
//	kdebugf2();
}

void UserListElement::operator = (const UserListElement &copyMe)
{
//	kdebugf();
//	printBacktrace("ULE::=\n");
	if (privateData->deref())
		delete privateData;
	privateData = copyMe.privateData;
	privateData->ref();
//	kdebugf2();
}

QVariant UserListElement::setData(const QString &name, const QVariant &val, bool massively, bool last)
{
	QVariant old;
	QVariant *oldVal = privateData->informations[name];
	if (oldVal)
		old = *oldVal;
	if (val == old)
		return old;
	if (oldVal)
		privateData->informations.replace(name, new QVariant(val));
	else
		privateData->informations.insert(name, new QVariant(val));
	delete oldVal;

	CONST_FOREACH (group, privateData->Parents)
		emit (*group)->userDataChanged(*this, name, old, val, massively, last);
	return old;
}

#define defineGetSetMethods(getMethod, setMethod, propertyName, type, returnMethod) \
	type UserListElement::getMethod() const \
	{ \
		return data(propertyName).returnMethod(); \
	} \
	void UserListElement::setMethod(const type &n) \
	{ \
		setData(propertyName, n); \
	}

defineGetSetMethods(homePhone,   setHomePhone, "HomePhone", QString,     toString)
defineGetSetMethods(firstName,   setFirstName, "FirstName", QString,     toString)
defineGetSetMethods(lastName,    setLastName,  "LastName",  QString,     toString)
defineGetSetMethods(nickName,    setNickName,  "NickName",  QString,     toString)
defineGetSetMethods(altNick,     setAltNick,   "AltNick",   QString,     toString)
defineGetSetMethods(mobile,      setMobile,    "Mobile",    QString,     toString)
defineGetSetMethods(email,       setEmail,     "Email",     QString,     toString)
defineGetSetMethods(isAnonymous, setAnonymous, "Anonymous", bool,        toBool)
defineGetSetMethods(notify,      setNotify,    "Notify",    bool,        toBool)

QString UserListElement::aliveSound(NotifyType &type) const
{
	type = (NotifyType) data("AliveSound").toInt();
	return data("OwnAliveSound").toString();
}

void UserListElement::setAliveSound(NotifyType type, const QString &file)
{
	if (type == data("AliveSound").toInt() && file == data("OwnAliveSound").toString())
		return;
	setData("OwnAliveSound", file);
	setData("AliveSound", int(type));
}

QString UserListElement::messageSound(NotifyType &type) const
{
	type = (NotifyType) data("MessageSound").toInt();
	return data("OwnMessageSound").toString();
}

void UserListElement::setMessageSound(NotifyType type, const QString &file)
{
	if (type == data("MessageSound").toInt() && file == data("OwnMessageSound").toString())
		return;
	setData("OwnMessageSound", file);
	setData("MessageSound", int(type));
}

bool UserListElement::usesProtocol(const QString &name) const
{
	return privateData->protocols.find(name) != NULL;
}

UserListKey UserListElement::key() const
{
	return privateData->key;
}

void UserListElement::addProtocol(const QString &name, const QString &id, bool massively, bool last)
{
#ifdef DEBUG_ENABLED
	if (privateData->protocols.find(name))
	{
		kdebugm(KDEBUG_ERROR, "%s found!!\n", name.local8Bit().data());
		printBacktrace("protocol already exists");
	}
#endif
	privateData->protocols.insert(name, new ProtocolData(name, id));
	CONST_FOREACH (group, privateData->Parents)
		emit (*group)->protocolAdded(*this, name, massively, last);
}

void UserListElement::deleteProtocol(const QString &protocolName, bool massively, bool last)
{
#ifdef DEBUG_ENABLED
	if (privateData->protocols.find(protocolName))
	{
		kdebugm(KDEBUG_PANIC, "%s not found!!\n", protocolName.local8Bit().data());
		printBacktrace("protocol does not exists");
	}
#endif
	CONST_FOREACH (group, privateData->Parents)
		emit (*group)->removingProtocol(*this, protocolName, massively, last);
	delete privateData->protocols[protocolName];
	privateData->protocols.remove(protocolName);
}

QVariant UserListElement::data(const QString &name) const
{
	QVariant *val = privateData->informations[name];
	if (val)
		return *val;
	else
		return QVariant();
}

void UserListElement::setDNSName(const QString &protocolName, const QString &dnsname)
{
	kdebugf();
	setProtocolData(protocolName, "DNSName", dnsname);
}

void UserListElement::setStatus(const QString &protocolName, const UserStatus &status, bool massively, bool last)
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return;
	}
	UserStatus *oldStatus = protoData->Stat->copy();
	*(protoData->Stat) = status;
	CONST_FOREACH (group, privateData->Parents)
		emit (*group)->statusChanged(*this, protocolName, *oldStatus, massively, last);
	delete oldStatus;
}

QStringList UserListElement::protocolList() const
{
	QDictIterator<ProtocolData> it(privateData->protocols);
	uint cnt = it.count();
	QStringList ret;
	for (uint i = 0; i < cnt; ++i)
	{
		ret += it.currentKey();
		++it;
	}
	return ret;
}

QStringList UserListElement::protocolDataKeys(const QString &protocolName) const
{
	ProtocolData *protoData = privateData->protocols.find(protocolName);
	if (protoData == NULL)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QStringList();
	}

	QDictIterator<QVariant> it(protoData->data);
	uint cnt = it.count();
	QStringList ret;
	for (uint i = 0; i < cnt; ++i)
	{
		ret += it.currentKey();
		++it;
	}
	return ret;
}

QStringList UserListElement::nonProtocolDataKeys() const
{
	QDictIterator<QVariant> it(privateData->informations);
	uint cnt = it.count();
	QStringList ret;
	for (uint i = 0; i < cnt; ++i)
	{
		ret += it.currentKey();
		++it;
	}
	return ret;
}
