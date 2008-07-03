/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QStringList>
#include <QVariant>

#include "debug.h"
#include "kadu.h"
#include "userlist.h"
#include "userlist-private.h"

#include "usergroup.h"

UserGroup::UserGroup()
	: QObject()
{
}

UserGroup::UserGroup(const UserGroup &copy)
	: QObject(), QSet<UserListElement>(copy)
{
}

UserGroup::UserGroup(const QList<UserListElement> &group)
	: QObject()
{
	addUsers(group);
}

//#include "kadu.h"
UserGroup::~UserGroup()
{
// 	int i = 1, cnt = count();
//	if (Kadu::closing())
//		blockSignals(true);
// 	if (cnt > 1)
// 		while (!privateUserGroupData->list.isEmpty())
// 			removeUser(privateUserGroupData->list[0], true, i++ == cnt);
// 	else if (cnt == 1)
// 		removeUser(privateUserGroupData->list[0]);
}

UserListElement UserGroup::byAltNick(const QString& altnick)
{
	foreach(const UserListElement &user, *this)
		if (user.altNick() == altnick)
			return user;

	kdebugm(KDEBUG_WARNING, "%s not found, creating ULE\n", altnick.local8Bit().data());
//	printBacktrace("ule nout found");
	UserListElement *newElem = new UserListElement();
	newElem->setAltNick(altnick);
	newElem->setAnonymous(true);
	addUser(*newElem);
	return *newElem;
}

bool UserGroup::containsAltNick(const QString &altnick, BehaviourForAnonymous beh) const
{
	foreach(const UserListElement &user, *this)
	{
		if (user.altNick() == altnick)
		{
			if (user.isAnonymous())
				return (beh == TrueForAnonymous);
			else
				return true;
		}
	}

	return false;
}

UserListElement UserGroup::byID(const QString &protocolName, const QString &id)
{
	foreach(const UserListElement &user, *this)
	{
		ProtocolData *protoData = *user.privateData->protocols.find(protocolName);
		if (protoData && protoData->ID == id)
			return user;
	}
	kdebugm(KDEBUG_WARNING, "%s,%s not found, creating ULE\n", protocolName.local8Bit().data(), id.local8Bit().data());
	return addAnonymous(protocolName, id);
}

bool UserGroup::contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh) const
{
	foreach(const UserListElement &user, *this)
	{
		if (!user.privateData->protocols.contains(protocolName))
			continue;

		ProtocolData *protoData = user.privateData->protocols[protocolName];
		if (protoData && (protoData->ID == id))
		{
			if (user.isAnonymous())
				return (beh == TrueForAnonymous);
			else
				return true;
		}
	}
	return false;
}

bool UserGroup::contains(const UserListElement &elem, BehaviourForAnonymous beh) const
{
//	kdebugf();
	if (QSet<UserListElement>::contains(elem))
		if (elem.isAnonymous())
			return (beh == TrueForAnonymous);
		else
			return true;
	else
		return false;
}

bool UserGroup::equals(const UserListElements users) const
{
	int cnt = count();
	if (cnt != users.count())
		return false;
	foreach(const UserListElement &user, users)
		if (!contains(user))
			return false;
	return true;
}

void UserGroup::addUser(const UserListElement &ule, bool massively, bool last)
{
//	if (last)
//	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' altNick:'%s' mass:%d last:%d\n", name(), ule.altNick().local8Bit().data(), massively, last);
	if (!ule.privateData->Parents.contains(this))
	{
		emit addingUser(ule, massively, last);
		insert(ule);
		ule.privateData->Parents.append(this);
		emit userAdded(ule, massively, last);
		emit modified();
	}
	if (!massively || (massively && last))
		emit usersAdded();
//	if (last)
//	kdebugf2();
}

UserListElement UserGroup::addAnonymous(const QString &protocolName, const QString &id, bool massively, bool last)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: proto:'%s' id:'%s' mass:%d\n", protocolName.local8Bit().data(), id.local8Bit().data(), massively);
	UserListElement e;
	e.setAltNick(id);
	e.setAnonymous(true);
	e.addProtocol(protocolName, id, massively, last);
	addUser(e, massively, last);
	kdebugf2();
	return e;
}

void UserGroup::addUsers(const UserGroup *group)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' group2:'%s'\n", name(), group->name());
	int i = 1, cnt = group->count();
	if (cnt > 1)
		foreach(const UserListElement &user, *group)
			addUser(user, true, i++ == cnt);
	else if (cnt == 1)
		addUser(*(group->constBegin()));
	kdebugf2();
}

void UserGroup::addUsers(QList<UserListElement> users)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s'\n", name());
	int i = 1, cnt = users.count();
	if (cnt > 1)
		foreach(const UserListElement &user, users)
			addUser(user, true, i++ == cnt);
	else if (cnt == 1)
		addUser(users[0]);
	kdebugf2();
}

void UserGroup::removeUsers(const UserGroup *group)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' group2:'%s'\n", name(), group->name());
	int i = 1, cnt = group->count();
	if (cnt > 1)
		foreach(const UserListElement &user, *group)
			removeUser(user, true, i++ == cnt);
	else if (cnt == 1)
		removeUser(*(group->constBegin()));
	kdebugf2();
}

void UserGroup::removeUsers(QList<UserListElement> users)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s'\n", name());
	int i = 1, cnt = users.count();
	if (cnt > 1)
		foreach(const UserListElement &user, users)
			removeUser(user, true, i++ == cnt);
	else if (cnt == 1)
		removeUser(users[0]);
	kdebugf2();
}

void UserGroup::removeUser(const UserListElement &ule, bool massively, bool last)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' altNick:'%s' mass:%d last:%d\n", name(), ule.altNick().local8Bit().data(), massively, last);
//	printBacktrace("xxx");

	if (contains(ule))
	{
		emit removingUser(ule, massively, last);
		// very ugly hack :|
		if (this == userlist && !Kadu::closing())
			ule.setAnonymous(true);
		else
		{
			ule.privateData->Parents.remove(this);
			QSet<UserListElement>::remove(ule);
		}
		emit userRemoved(ule, massively, last);
		emit modified();
	}

	if (!massively || (massively && last))
		emit usersRemoved();

//	kdebugf2();
}

UserListElements UserGroup::toUserListElements() const
{
	UserListElements result;
	foreach (const UserListElement &user, *this)
		result.append(user);
	return result;
}

void UserGroup::clear()
{
	removeUsers(toUserListElements());
// 	removeUsers(this);
}

QStringList UserGroup::altNicks() const
{
	QStringList nicks;
	foreach(const UserListElement &user, *this)
		nicks.append(user.altNick());
	return nicks;
}

bool UserListElements::equals(const UserListElements &elems) const
{
	if (count() != elems.count())
		return false;
	foreach(const UserListElement &user, elems)
		if (!contains(user))
			return false;
	return true;
}

bool UserListElements::equals(const UserGroup *group) const
{
	if (count() != group->count())
		return false;
	foreach(const UserListElement &user, *this)
		if (!group->contains(user))
			return false;
	return true;
}

void UserListElements::sort()
{
	if (count() > 1)
		qSort(*this);
}

UserListElements::UserListElements(UserListElement u)
{
	append(u);
}

UserListElements::UserListElements(const UserListElements &u) : QList<UserListElement>(u)
{
}

UserListElements::UserListElements(const QList<UserListElement> &u) : QList<UserListElement>(u)
{
}

UserListElements::UserListElements()
{
}

bool UserListElements::contains(QString protocol, QString id) const
{
	foreach(const UserListElement &user, *this)
		if (user.usesProtocol(protocol) && user.ID(protocol) == id)
			return true;
	return false;
}

QStringList UserListElements::altNicks() const
{
	QStringList nicks;
	foreach(const UserListElement &user, *this)
		nicks.append(user.altNick());
	return nicks;
}

bool UserListElements::operator < (const UserListElements &compareTo) const
{
	if (count() < compareTo.count())
		return true;

	if (count() > compareTo.count())
		return false;

	for (int i = 0; i < count(); i++)
	{
		if (at(i) < compareTo.at(i))
			return true;
		if (compareTo.at(i) < at(i))
			return false;
	}

	return false;
}
