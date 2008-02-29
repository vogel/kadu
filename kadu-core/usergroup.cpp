/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "debug.h"
#include "kadu.h"
//CONST_FOREACH
#include "misc.h"

#include "usergroup.h"
#include "userlist.h"
#include "userlist-private.h"
//Added by qt3to4:
#include <QList>

UserGroup::UserGroup(int size, const char *name) : QObject(NULL, name), d(new UserGroupData(size))
{
}

UserGroup::UserGroup(const QList<UserListElement> &group, const char *name) : QObject(NULL, name), d(new UserGroupData(group.size() * 2))
{
	addUsers(group);
}

//#include "kadu.h"
UserGroup::~UserGroup()
{
	int i = 1, cnt = count();
//	if (Kadu::closing())
//		blockSignals(true);
	if (cnt > 1)
		while (!d->list.isEmpty())
			removeUser(d->list[0], true, i++ == cnt);
	else if (cnt == 1)
		removeUser(d->list[0]);
	delete d;
}

UserListElement UserGroup::byAltNick(const QString& altnick)
{
	CONST_FOREACH(i, d->list)
		if ((*i).altNick() == altnick)
			return *i;
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
	CONST_FOREACH(i, d->list)
		if ((*i).altNick() == altnick)
			if ((*i).isAnonymous())
				return (beh == TrueForAnonymous);
			else
				return true;

	return false;
}

UserListElement UserGroup::byID(const QString &protocolName, const QString &id)
{
	CONST_FOREACH(ule, d->list)
	{
		ProtocolData *protoData = (*ule).privateData->protocols.find(protocolName);
		if (protoData && protoData->ID == id)
			return *ule;
	}
	kdebugm(KDEBUG_WARNING, "%s,%s not found, creating ULE\n", protocolName.local8Bit().data(), id.local8Bit().data());
	return addAnonymous(protocolName, id);
}

bool UserGroup::contains(const QString &protocolName, const QString &id, BehaviourForAnonymous beh) const
{
	CONST_FOREACH(ule, d->list)
	{
		ProtocolData *protoData = (*ule).privateData->protocols.find(protocolName);
		if (protoData && protoData->ID == id)
			if ((*ule).isAnonymous())
				return (beh == TrueForAnonymous);
			else
				return true;
	}
	return false;
}

bool UserGroup::contains(UserListElement elem, BehaviourForAnonymous beh) const
{
//	kdebugf();
	if (d->data.find(elem.key()) != NULL)
		if (elem.isAnonymous())
			return (beh == TrueForAnonymous);
		else
			return true;
	else
		return false;
}

bool UserGroup::equals(UserListElements users) const
{
	int cnt = count();
	if (cnt != users.count())
		return false;
	CONST_FOREACH(user, users)
		if (!contains(*user))
			return false;
	return true;
}

bool UserGroup::equals(const UserGroup *group) const
{
	if (NULL == group)
		return false;
	if (this == group)
		return true;
	unsigned int cnt = count();
	if (cnt != group->count())
		return false;
	CONST_FOREACH(user, *group)
		if (!contains(*user))
			return false;
	return true;
}

UserListElement UserGroup::byKey(UserListKey key)
{
	return *(d->data[key]);
}

void UserGroup::addUser(UserListElement ule, bool massively, bool last)
{
//	if (last)
//	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' altNick:'%s' mass:%d last:%d\n", name(), ule.altNick().local8Bit().data(), massively, last);
	if (!ule.privateData->Parents.contains(this))
	{
		emit addingUser(ule, massively, last);
		d->data.insert(ule.key(), new UserListElement(ule));
		d->list.append(ule);
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
		CONST_FOREACH(user, *group)
			addUser(*user, true, i++ == cnt);
	else if (cnt == 1)
		addUser(*(group->constBegin()));
	kdebugf2();
}

void UserGroup::addUsers(QList<UserListElement> users)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s'\n", name());
	int i = 1, cnt = users.count();
	if (cnt > 1)
		CONST_FOREACH(user, users)
			addUser(*user, true, i++ == cnt);
	else if (cnt == 1)
		addUser(users[0]);
	kdebugf2();
}

void UserGroup::removeUsers(const UserGroup *group)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' group2:'%s'\n", name(), group->name());
	int i = 1, cnt = group->count();
	if (cnt > 1)
		CONST_FOREACH(user, *group)
			removeUser(*user, true, i++ == cnt);
	else if (cnt == 1)
		removeUser(*(group->constBegin()));
	kdebugf2();
}

void UserGroup::removeUsers(QList<UserListElement> users)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s'\n", name());
	int i = 1, cnt = users.count();
	if (cnt > 1)
		CONST_FOREACH(user, users)
			removeUser(*user, true, i++ == cnt);
	else if (cnt == 1)
		removeUser(users[0]);
	kdebugf2();
}

void UserGroup::removeUser(UserListElement ule, bool massively, bool last)
{
//	kdebugmf(KDEBUG_FUNCTION_START, "start: group:'%s' altNick:'%s' mass:%d last:%d\n", name(), ule.altNick().local8Bit().data(), massively, last);
//	printBacktrace("xxx");
	UserListElement *elem = d->data.find(ule.key());
	if (elem != NULL)
	{
//		kdebugm(KDEBUG_INFO, "user found\n");
		emit removingUser(ule, massively, last);
		// very ugly hack :|
		if (this == userlist && !Kadu::closing())
			ule.setAnonymous(true);
		else
		{
			ule.privateData->Parents.remove(this);
			d->data.remove(ule.key());
			d->list.remove(ule);
			delete elem;
		}
		emit userRemoved(ule, massively, last);
		emit modified();
	}
	if (!massively || (massively && last))
		emit usersRemoved();

//	kdebugf2();
}

UserGroup::size_type UserGroup::count() const
{
	return d->list.count();
}

UserGroup::const_iterator UserGroup::constBegin () const
{
	return d->list.constBegin();
}

UserGroup::const_iterator UserGroup::constEnd () const
{
	return d->list.constEnd();
}

UserGroup::iterator UserGroup::begin () const
{
	return d->list.begin();
}

UserGroup::iterator UserGroup::end () const
{
	return d->list.end();
}

UserListElements UserGroup::toUserListElements() const
{
	return d->list;
}

void UserGroup::resize(int size)
{
	d->data.resize(size);
}

void UserGroup::clear()
{
	removeUsers(toUserListElements());
// 	removeUsers(this);
}

QStringList UserGroup::altNicks() const
{
	QStringList nicks;
	CONST_FOREACH(user, d->list)
		nicks.append((*user).altNick());
	return nicks;
}

bool UserListElements::equals(const UserListElements &elems) const
{
	if (count() != elems.count())
		return false;
	CONST_FOREACH(user, elems)
		if (!contains(*user))
			return false;
	return true;
}

bool UserListElements::equals(const UserGroup *group) const
{
	if (count() != group->count())
		return false;
	CONST_FOREACH(user, *this)
		if (!group->contains(*user))
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
	CONST_FOREACH(user, *this)
		if ((*user).usesProtocol(protocol) && (*user).ID(protocol) == id)
			return true;
	return false;
}

QStringList UserListElements::altNicks() const
{
	QStringList nicks;
	CONST_FOREACH(user, *this)
		nicks.append((*user).altNick());
	return nicks;
}

bool UserListElements::operator < (const UserListElements &compareTo) const
{
	if (count() < compareTo.count())
		return true;

	if (count() > compareTo.count())
		return false;

	for (unsigned int i = 0; i < count(); i++)
	{
		if (at(i) < compareTo.at(i))
			return true;
		if (compareTo.at(i) < at(i))
			return false;
	}

	return false;
}
