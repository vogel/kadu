/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtNetwork/QHostAddress>

#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "misc.h"
#include "status.h"
#include "usergroup.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "userlistelement.h"
#include "userlist-private.h"

bool UserListElement::hasIP(const QString &protocolName) const
{
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return false;
	}

	bool result = privateData->protocols[protocolName].data.contains("IP") && privateData->protocols[protocolName].data["IP"].toUInt() != 0;
	//privateData->unlock();
	return result;
}

QHostAddress UserListElement::IP(const QString &protocolName) const
{
	return QHostAddress(protocolData(protocolName, "IP").toUInt());
}

QString UserListElement::DNSName(const QString &protocolName) const
{
	return protocolData(protocolName, "DNSName").toString();
}

void UserListElement::setAddressAndPort(const QString &protocolName, const QHostAddress &ip, unsigned short port) const
{
	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return;
	}

	setDNSName(protocolName, QString::null);
	setProtocolData(protocolName, "IP", ip.ip4Addr());
	setProtocolData(protocolName, "Port", port);

	//privateData->unlock();
}

unsigned short UserListElement::port(const QString &protocolName) const
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
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return QVariant();
	}

	ProtocolData &protoData = privateData->protocols[protocolName];
	QVariant old;

	if (protoData.data.contains(name))
		if (protoData.data[name] == val)
		{
			//privateData->unlock();
			return protoData.data[name];
		}
		else
			old = protoData.data[name];

	protoData.data[name] = val;

	if (massively)
	{
		QHash<QString, UserGroupSet> &names = ULEPrivate::protocolUserDataProxy[protocolName];
		UserGroupSet &groups = names[name];

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

	QVariant result = protoData.data[name];
	//privateData->unlock();

	return result;
}

QVariant UserListElement::protocolData(const QString &protocolName, const QString &name) const
{
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return QVariant();
	}

	QVariant result;
	if (privateData->protocols[protocolName].data.contains(name))
		result = privateData->protocols[protocolName].data.value(name);

	//privateData->unlock();
	return result;
}

QString UserListElement::ID(const QString &protocolName) const
{
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return QString::null;
	}

	QString result = privateData->protocols[protocolName].ID;
	//privateData->unlock();
	return result;
}

UserListElement::UserListElement(const UserListElement &copyMe)
	: QObject()
{
// 	kdebugf();

	privateData = copyMe.privateData;

//	kdebugf2();
}

UserListElement::UserListElement()
	: QObject(), privateData(new ULEPrivate())
{
}

UserListElement::~UserListElement()
{
}

UserListElement & UserListElement::operator = (const UserListElement &copyMe)
{
	privateData = copyMe.privateData;

	return *this;
}

QVariant UserListElement::setData(const QString &name, const QVariant &val, bool massively, bool last) const
{
//	kdebugf();

	//privateData->lock();

	QVariant old;

	if (privateData->informations.contains(name))
		if (privateData->informations[name] == val)
		{
			//privateData->unlock();
			return privateData->informations[name];
		}
		else
			old = privateData->informations[name];

	privateData->informations[name] = val;

	if (massively)
	{
		UserGroupSet groups;

		if (ULEPrivate::userDataProxy.find(name) == ULEPrivate::userDataProxy.end())
			ULEPrivate::userDataProxy[name] = groups;
		else
			groups = *ULEPrivate::userDataProxy.find(name);

		foreach (UserGroup *group, privateData->Parents)
		{
			emit group->userDataChanged(*this, name, old, val, massively, last);
			if (!groups.contains(group))
				groups.insert(group);
		}
		if (last)
		{
			foreach (UserGroup *ug, groups)
				emit ug->usersDataChanged(name);

			groups.clear();
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

	QVariant result = val;
	//privateData->unlock();
	return val;
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
//	kdebugf();

	//privateData->lock();
	bool result = privateData->protocols.contains(name);
	//privateData->unlock();
	return result;
}

void UserListElement::addProtocol(const QString &name, const QString &id, bool massively, bool last) const
{
//	kdebugf();

	//privateData->lock();

	if (privateData->protocols.contains(name))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s found!!\n", qPrintable(name));
		printBacktrace("protocol already exists");
#endif
		//privateData->unlock();
		return;
	}

	privateData->protocols.insert(name, ProtocolData(name, id));
	foreach (UserGroup *group, privateData->Parents)
		emit group->protocolAdded(*this, name, massively, last);

	//privateData->unlock();
}

void UserListElement::deleteProtocol(const QString &protocolName, bool massively, bool last) const
{
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_PANIC, "%s not found!!\n", qPrintable(protocolName));
		printBacktrace("protocol does not exists");
#endif
		//privateData->unlock();
		return;
	}

	foreach (UserGroup *group, privateData->Parents)
		emit group->removingProtocol(*this, protocolName, massively, last);
	privateData->protocols.remove(protocolName);

	//privateData->unlock();
}

QVariant UserListElement::data(const QString &name) const
{
//	kdebugf();

	//privateData->lock();

	QVariant result;
	if (privateData->informations.contains(name))
		result = privateData->informations[name];

	//privateData->unlock();
	return result;
}

void UserListElement::setDNSName(const QString &protocolName, const QString &dnsname) const
{
	kdebugf();
	setProtocolData(protocolName, "DNSName", dnsname);
}

void UserListElement::setStatus(const QString &protocolName, const UserStatus &status, bool massively, bool last) const
{
//	kdebugf();

	//privateData->lock();
	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return;
	}

	UserStatus oldStatus = *privateData->protocols[protocolName].Stat->Stat;
	privateData->protocols[protocolName].Stat->Stat->setStatus(status);

	if (massively)
	{
		UserGroupSet &groups = ULEPrivate::statusChangeProxy[protocolName];

		foreach (UserGroup *group, privateData->Parents)
		{
//			kdebugm(KDEBUG_INFO, "group: %p\n", *group);
 			emit group->statusChanged(*this, protocolName, oldStatus, massively, last);
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
 			emit group->statusChanged(*this, protocolName, oldStatus, massively, last);
			emit group->usersStatusChanged(protocolName);
		}
	}

	//privateData->unlock();
//	kdebugf2();
}

QStringList UserListElement::protocolList() const
{
	kdebugf();

	//privateData->lock();
	QStringList result = privateData->protocols.keys();
	//privateData->unlock();
	return result;
}

QStringList UserListElement::protocolDataKeys(const QString &protocolName) const
{
//	kdebugf();

	//privateData->lock();

	if (!privateData->protocols.contains(protocolName))
	{
#ifdef DEBUG_ENABLED
		kdebugm(KDEBUG_ERROR, "%s not found!\n", qPrintable(protocolName));
		printBacktrace("backtrace");
#endif
		//privateData->unlock();
		return QStringList();
	}

	QStringList result = privateData->protocols[protocolName].data.keys();
	//privateData->unlock();
	return result;
}

QStringList UserListElement::nonProtocolDataKeys() const
{
	kdebugf();

	//privateData->lock();
	QStringList result = privateData->informations.keys();
	//privateData->unlock();
	return result;
}

bool UserListElement::containsGroup(UserGroup *group) const
{
	return privateData->Parents.contains(group);
}

void UserListElement::addGroup(UserGroup *group) const
{
	privateData->Parents.append(group);
}

void UserListElement::removeGroup(UserGroup *group) const
{
	privateData->Parents.remove(group);
}

ulong qHash(const UserListElement &index)
{
	return (ulong)index.privateData.data();
}

Contact UserListElement::toContact(Account *account) const
{
	return ContactManager::instance()->getContactById(account, ID("Gadu"));
}

UserListElement UserListElement::fromContact(Contact contact, Account *account)
{
	ContactAccountData *data = contact.accountData(account);

	printf("UserListElement::fromContact: %p\n", data);

	if (data)
		return userlist->byID("Gadu", data->id());
	else
		return UserListElement();
}
