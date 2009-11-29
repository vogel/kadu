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

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Account;
class Group;
class XmlConfigFile;

class KADUAPI BuddyManager : public QObject, public SimpleManager<Buddy>
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyManager)

	static BuddyManager * Instance;

	BuddyManager();
	virtual ~BuddyManager();

	void init();

	const Buddy & byBuddyShared(BuddyShared *data);
	void importConfiguration(XmlConfigFile *configurationStorage);

private slots:
	void buddyDataUpdated();
	void contactAboutToBeAdded(Account account);
	void contactAdded(Account account);
	void contactAboutToBeRemoved(Account account);
	void contactRemoved(Account account);
	void contactIdChanged(Account account, const QString &oldId);

	void groupRemoved(Group group);

protected:
	virtual QString configurationNodeName() { return QLatin1String("Buddies"); }
	virtual QString configurationNodeItemName() { return QLatin1String("Buddy"); }

	virtual void itemAboutToBeAdded(Buddy buddy);
	virtual void itemAdded(Buddy buddy);
	virtual void itemAboutToBeRemoved(Buddy buddy);
	virtual void itemRemoved(Buddy buddy);

public:
	static BuddyManager * instance();

	virtual void load();

	BuddyList buddies(Account account, bool includeAnonymous = false);
	void mergeBuddies(Buddy destination, Buddy source);

	Buddy byId(Account account, const QString &id);
	Buddy byDisplay(const QString &display);

	void blockUpdatedSignal(Buddy &buddy);
	void unblockUpdatedSignal(Buddy &buddy);

signals:
	void buddyAboutToBeAdded(Buddy &buddy);
	void buddyAdded(Buddy &buddy);
	void buddyAboutToBeRemoved(Buddy &buddy);
	void buddyRemoved(Buddy &buddy);

	void buddyUpdated(Buddy &buddy);
	void contactAboutToBeAdded(Buddy &buddy, Account account);
	void contactAdded(Buddy &buddy, Account account);
	void contactAboutToBeRemoved(Buddy &buddy, Account account);
	void contactRemoved(Buddy &buddy, Account account);
	void contactIdChanged(Buddy &buddy, Account account, const QString &oldId);

};

#endif // BUDDY_MANAGER_H
