/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QHostAddress>

#include "debug.h"
#include "misc.h"
#include "status.h"
#include "usergroup.h"

#include "userlistelement.h"
#include "userlist-private.h"

bool UserListElement::hasIP(const QString &protocolName) const
{
	ProtocolData *protoData = *privateData->protocols.find(protocolName);
	if (!protoData)
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return false;
	}
	return protoData->data.contains("IP") && protoData->data["IP"]->toUInt() != 0;
}

QHostAddress UserListElement::IP(const QString &protocolName) const
{
	return QHostAddress(protocolData(protocolName, "IP").toUInt());
}

QString UserListElement::DNSName(const QString &protocolName) const
{
	return protocolData(protocolName, "DNSName").toString();
}

void UserListElement::setAddressAndPort(const QString &protocolName, const QHostAddress &ip, short port) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return;
	}
	setDNSName(protocolName, QString::null);
	setProtocolData(protocolName, "IP", ip.ip4Addr());
	setProtocolData(protocolName, "Port", port);
}

short UserListElement::port(const QString &protocolName) const
{
	return protocolData(protocolName, "Port").toUInt();
}

void UserListElement::refreshDNSName(const QString &protocolName) const
{
// TODO: 0.6.5
// 	unsigned int ip = protocolData(protocolName, "IP").toUInt();
// 	if (ip)
// 		connect(new DNSHandler(protocolName, ip), SIGNAL(result(const QString &, const QString &)),
// 				privateData, SLOT(setDNSName(const QString &, const QString &)));
}

QVariant UserListElement::setProtocolData(const QString &protocolName, const QString &name, const QVariant &val, bool massively, bool last) const
{
	ProtocolData *protoData = *privateData->protocols.find(protocolName);

	if (!protoData)
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
		protoData->data[name] = new QVariant(val);
	else
		protoData->data[name] = new QVariant(val);
	delete oldVal;

	if (massively)
	{
		QHash<QString, UserGroupSet> names = ULEPrivate::protocolUserDataProxy[protocolName];
		UserGroupSet groups = names[name];

		foreach (UserGroup *group, privateData->Parents)
		{
			emit group->protocolUserDataChanged(protocolName, *this, name, old, val, massively, last);
			if (!groups.contains(group))
				groups.insert(group);
		}
		if (last)
		{
			foreach (UserGroup *ug, groups)
				emit ug->protocolUsersDataChanged(protocolName, name);

			groups.clear();
		}
	}
	else
	{
		foreach (UserGroup *group, privateData->Parents)
		{
			emit group->protocolUserDataChanged(protocolName, *this, name, old, val, massively, last);
			emit group->protocolUsersDataChanged(protocolName, name);
		}
	}

	return old;
}

QVariant UserListElement::protocolData(const QString &protocolName, const QString &name) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QVariant();
	}

	QVariant *val = privateData->protocols[protocolName]->data.value(name);
	if (val)
		return *val;
	else
		return QVariant();
}

const UserStatus & UserListElement::status(const QString &protocolName) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		static UserStatus us;
		return us;
	}
	return privateData->protocols[protocolName]->Stat;
}

QString UserListElement::ID(const QString &protocolName) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QString::null;
	}

	return privateData->protocols[protocolName]->ID;
}

UserListElement::UserListElement(const UserListElement &copyMe)
	: QObject()
{
	kdebugf();
// 	copyMe.privateData->ref();
	privateData = copyMe.privateData;
	kdebugf2();
}

UserListElement::UserListElement()
	: QObject(), privateData(new ULEPrivate())
{
//	kdebugf();
	privateData->key = used ++;
// 	privateData->ref();
//	kdebugf2();
}

UserListElement::~UserListElement()
{
//	kdebugf();
// 	if (privateData->deref())
// 	{
// 		delete privateData;
// 		privateData = 0;
// 	}
//	kdebugf2();
}

UserListElement &UserListElement::operator = (const UserListElement &copyMe)
{
//	kdebugf();
//	printBacktrace("ULE::=\n");
// 	if (privateData->deref())
// 		delete privateData;
// 	privateData = copyMe.privateData;
// 	privateData->ref();
//	kdebugf2();
	privateData = copyMe.privateData;
	return *this;
}

QVariant UserListElement::setData(const QString &name, const QVariant &val, bool massively, bool last) const
{
	QVariant old;
	QVariant *oldVal = privateData->informations[name];
	if (oldVal)
		old = *oldVal;
	if (val == old)
		return old;
	privateData->informations[name] = new QVariant(val);
	delete oldVal;

	if (massively)
	{
		UserGroupSet *groups = &*ULEPrivate::userDataProxy.find(name);
		if (!groups)
		{
			groups = new UserGroupSet();
			ULEPrivate::userDataProxy[name] = *groups;
		}
		foreach (UserGroup *group, privateData->Parents)
		{
			emit group->userDataChanged(*this, name, old, val, massively, last);
			if (!groups->contains(group))
				groups->insert(group);
		}
		if (last)
		{
			foreach (UserGroup *ug, *groups)
				emit ug->usersDataChanged(name);

			groups->clear();
		}
	}
	else
	{
		foreach (UserGroup *group, privateData->Parents)
		{
			emit group->userDataChanged(*this, name, old, val, massively, last);
			emit group->usersDataChanged(name);
		}
	}
	return old;
}

#define defineGetSetMethods(getMethod, setMethod, propertyName, type, returnMethod) \
	type UserListElement::getMethod() const \
	{ \
		return data(propertyName).returnMethod(); \
	} \
	void UserListElement::setMethod(const type &n) const \
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

void UserListElement::setAliveSound(NotifyType type, const QString &file) const
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

void UserListElement::setMessageSound(NotifyType type, const QString &file) const
{
	if (type == data("MessageSound").toInt() && file == data("OwnMessageSound").toString())
		return;
	setData("OwnMessageSound", file);
	setData("MessageSound", int(type));
}

bool UserListElement::usesProtocol(const QString &name) const
{
	return privateData->protocols.contains(name);
}

void UserListElement::addProtocol(const QString &name, const QString &id, bool massively, bool last) const
{
	if (privateData->protocols.contains(name))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s found!!\n", name.local8Bit().data());
		printBacktrace("protocol already exists");
#endif
		return;
	}
	privateData->protocols.insert(name, new ProtocolData(name, id));
	foreach (UserGroup *group, privateData->Parents)
		emit group->protocolAdded(*this, name, massively, last);
}

void UserListElement::deleteProtocol(const QString &protocolName, bool massively, bool last) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_PANIC, "%s not found!!\n", protocolName.local8Bit().data());
		printBacktrace("protocol does not exists");
#endif
		return;
	}
	foreach (UserGroup *group, privateData->Parents)
		emit group->removingProtocol(*this, protocolName, massively, last);
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

void UserListElement::setDNSName(const QString &protocolName, const QString &dnsname) const
{
	kdebugf();
	setProtocolData(protocolName, "DNSName", dnsname);
}

void UserListElement::setStatus(const QString &protocolName, const UserStatus &status, bool massively, bool last) const
{
//	kdebugf();
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return;
	}

	UserStatus *oldStatus = privateData->protocols[protocolName]->Stat->copy();
	*privateData->protocols[protocolName]->Stat = status;

	if (massively)
	{
		UserGroupSet groups = ULEPrivate::statusChangeProxy[protocolName];

		foreach (UserGroup *group, privateData->Parents)
		{
//			kdebugm(KDEBUG_INFO, "group: %p\n", *group);
 			emit group->statusChanged(*this, protocolName, *oldStatus, massively, last);
			if (!groups.contains(group))
				groups.insert(group);
		}
		if (last)
		{
			foreach (UserGroup *ug, groups)
				emit ug->usersStatusChanged(protocolName);

			groups.clear();
		}
	}
	else
	{
		foreach (UserGroup *group, privateData->Parents)
		{
 			emit group->statusChanged(*this, protocolName, *oldStatus, massively, last);
			emit group->usersStatusChanged(protocolName);
		}
	}

	delete oldStatus;
//	kdebugf2();
}

QStringList UserListElement::protocolList() const
{
	return privateData->protocols.keys();
}

QStringList UserListElement::protocolDataKeys(const QString &protocolName) const
{
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", protocolName.local8Bit().data());
		printBacktrace("backtrace");
#endif
		return QStringList();
	}

	return privateData->protocols[protocolName]->data.keys();
}

QStringList UserListElement::nonProtocolDataKeys() const
{
	return privateData->informations.keys();
}

uint qHash(const UserListElement &index)
{
	return (uint)index.privateData.data();
}
