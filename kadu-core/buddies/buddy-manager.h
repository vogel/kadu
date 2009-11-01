/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_MANAGER_H
#define BUDDY_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/storable-object.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"

#include "exports.h"

class Account;
class Group;
class XmlConfigFile;

class KADUAPI BuddyManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyManager)

	static BuddyManager * Instance;

	BuddyList Contacts;

	BuddyManager();
	virtual ~BuddyManager();

	const Buddy & byBuddyShared(BuddyShared *data);
	void importConfiguration(XmlConfigFile *configurationStorage);

private slots:
	void buddyDataUpdated();
	void contactAccountDataAboutToBeAdded(Account account);
	void contactAccountDataAdded(Account account);
	void contactAccountDataAboutToBeRemoved(Account account);
	void contactAccountDataRemoved(Account account);
	void contactAccountDataIdChanged(Account account, const QString &oldId);

	void groupRemoved(Group *group);

protected:
	virtual StoragePoint * createStoragePoint();

public:
	static BuddyManager * instance();

	virtual void load();
	virtual void store();

	BuddyList buddies();
	BuddyList buddies(Account account, bool includeAnonymous = false);
	void addBuddy(Buddy contact);
	void removeBuddy(Buddy contact);
	void mergeBuddies(Buddy destination, Buddy source);

	unsigned int count() { return Contacts.count(); }

	Buddy byIndex(unsigned int index);
	int contactIndex(Buddy contact) { return Contacts.indexOf(contact); }

	Buddy byId(Account account, const QString &id);
	Buddy byUuid(const QString &uuid);
	Buddy byDisplay(const QString &display);

	void blockUpdatedSignal(Buddy &contact);
	void unblockUpdatedSignal(Buddy &contact);

signals:
	void buddyAboutToBeAdded(Buddy &contact);
	void buddyAdded(Buddy &contact);
	void buddyAboutToBeRemoved(Buddy &contact);
	void buddyRemoved(Buddy &contact);

	void buddyUpdated(Buddy &contact);
	void contactAccountDataAboutToBeAdded(Buddy &contact, Account account);
	void contactAccountDataAdded(Buddy &contact, Account account);
	void contactAccountDataAboutToBeRemoved(Buddy &contact, Account account);
	void contactAccountDataRemoved(Buddy &contact, Account account);
	void contactAccountIdChanged(Buddy &contact, Account account, const QString &oldId);

};

#endif // BUDDY_MANAGER_H
