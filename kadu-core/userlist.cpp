/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//TODO and ideas:
// list of protocols for userlist?
// functions for easier searching of altnicks/protocols+ids?
// "plugins" for changing the way userboxes are painted? maybe only parameters?
// later ULE::hasFeature(protocol, featurename)

#include <QtCore/QFile>
#include <QtXml/QDomNodeList>

extern "C" int gadu_protocol_init(bool firstLoad);

#include "accounts/account_manager.h"

#include "contacts/contact-manager.h"

#include "protocols/protocol.h"
#include "protocols/protocols_manager.h"

#include "config_file.h"
#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "misc.h"
#include "xml_config_file.h"

#include "userlist.h"
#include "userlist-private.h"

UserList::UserList()
	: UserGroup(), nonProtoKeys(), protoKeys()
{
	initKeys();
}

UserList::~UserList()
{
}

void UserList::merge(const QList<UserListElement> &ulist)
{
	kdebugf();
	UserListElements toAppend;
	UserListElements toUnsetAnonymous;

	foreach(const UserListElement &user, ulist)
	{
		UserListElement user2;
		if (containsAltNick(user.altNick()))
			user2 = byAltNick(user.altNick());
		else
		{
			QStringList protos = user.protocolList();
			QString foundProto;

			foreach(const QString &proto, protos)
				if (contains(proto, user.ID(proto)))
				{
					foundProto = proto;
					break;
				}

			if (!foundProto.isEmpty())
				user2 = byID(foundProto, user.ID(foundProto));
			else
			{
				toAppend.append(user);
				continue;
			}
		}

		//copying of protocols
		QStringList protos = user.protocolList();

		foreach(const QString &proto, protos)
		{
			if (!contains(proto, user.ID(proto)))
				user2.addProtocol(proto, user.ID(proto));

			//and protocols data
			UserListElement user3 = byID(proto, user.ID(proto));
			QStringList protoDataKeys = user.protocolDataKeys(proto);

			foreach(const QString &key, protoDataKeys)
			{
				QVariant val = user3.protocolData(proto, key);
				if (!val.isValid() || val.isNull())
					user3.setProtocolData(proto, key, user.protocolData(proto, key));
			}
		}

		//copying of non protocol data
		QStringList dataKeys = user.nonProtocolDataKeys();

		foreach(const QString &key, dataKeys)
		{
			QVariant val = user2.data(key);
			if (!val.isValid() || val.isNull())
				user2.setData(key, user.data(key));
		}

		if (user2.isAnonymous())
		{
			user2.setAltNick(user.altNick());
			user2.setEmail(user.email());
			user2.setHomePhone(user.homePhone());
			user2.setNotify(user.notify());
			user2.setMobile(user.mobile());
			user2.setNickName(user.nickName());
			user2.setLastName(user.lastName());
			user2.setFirstName(user.firstName());
			user2.setData("Groups", user.data("Groups"));

			NotifyType nt;
			QString path;

			path = user2.aliveSound(nt);
			user2.setAliveSound(nt, path);

			path = user2.messageSound(nt);
			user2.setMessageSound(nt, path);

			toUnsetAnonymous.append(user2);
		}
	}

	int i = 1, anonSize = toUnsetAnonymous.size();

	foreach(const UserListElement &user, toUnsetAnonymous)
		user.setData("Anonymous", false, true, i++ == anonSize);

	addUsers(toAppend);

	emit modified();
	kdebugf2();
}

void UserList::clear()
{
	removeUsers(toUserListElements());
}

void UserList::initModule()
{
	userlist = new UserList();
}

void UserList::closeModule()
{
	delete userlist;
	userlist = NULL;
	ULEPrivate::closeModule();
}

void UserList::addPerContactNonProtocolConfigEntry(const QString &attribute_name, const QString &internal_key)
{
	kdebugf();

	if (nonProtoKeys.find(attribute_name) != nonProtoKeys.end())
	{
		kdebugf2();
		return;
	}
	nonProtoKeys[attribute_name] = internal_key;

	QStringList keys = QStringList::split(",", config_file.readEntry("General", "NonProtoAdditionalAttributes"));
	keys.append(attribute_name + '=' + internal_key);
	config_file.writeEntry("General", "NonProtoAdditionalAttributes", keys.join(","));

	kdebugf2();
}

void UserList::removePerContactNonProtocolConfigEntry(const QString &attribute_name)
{
	kdebugf();

	QMap<QString, QString>::iterator it = nonProtoKeys.find(attribute_name);
	if (it == nonProtoKeys.end())
		return;
	nonProtoKeys.erase(it);

	QStringList keys = QStringList::split(",", config_file.readEntry("General", "NonProtoAdditionalAttributes"));
	QStringList atts = keys.grep(attribute_name + '=');
	foreach(const QString &att, atts)
		keys.remove(att);
	config_file.writeEntry("General", "NonProtoAdditionalAttributes", keys.join(","));

	kdebugf2();
}

void UserList::addPerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name, const QString &internal_key)
{
	kdebugf();
	QMap<QString, QString> &p = protoKeys[protocolName];
	if (p.find(attribute_name) != p.end())
	{
		kdebugf2();
		return;
	}
	p[attribute_name] = internal_key;

	QStringList keys = QStringList::split(",", config_file.readEntry("General", "ProtoAdditionalAttributes"));
	keys.append(protocolName + '=' + attribute_name + '=' + internal_key);
	config_file.writeEntry("General", "ProtoAdditionalAttributes", keys.join(","));
	kdebugf2();
}

void UserList::removePerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name)
{
	kdebugf();
	QMap<QString, QString> &p = protoKeys[protocolName];
	QMap<QString, QString>::iterator it = p.find(attribute_name);
	if (it != p.end())
	{
		kdebugf2();
		return;
	}
	p.erase(it);

	QStringList keys = QStringList::split(",", config_file.readEntry("General", "ProtoAdditionalAttributes"));
	QStringList atts = keys.grep(protocolName + '=' + attribute_name + '=');
	foreach(const QString &att, atts)
		keys.remove(att);
	config_file.writeEntry("General", "ProtoAdditionalAttributes", keys.join(","));

	kdebugf2();
}

void UserList::initKeys()
{
	kdebugf();

	QStringList keys1 = QStringList::split(",", config_file.readEntry("General", "NonProtoAdditionalAttributes"));
	foreach(const QString &it, keys1)
	{
		QStringList x = QStringList::split("=", it);
		nonProtoKeys[x[0]] = x[1];
	}

	QStringList keys2 = QStringList::split(",", config_file.readEntry("General", "ProtoAdditionalAttributes"));
	foreach(const QString &it, keys2)
	{
		QStringList x = QStringList::split("=", it);
		protoKeys[x[0]][x[1]] = x[2];
	}

	kdebugf2();
}

UserList *userlist;
