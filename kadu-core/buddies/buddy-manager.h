/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Dawid Stawiarski (neeo@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BUDDY_MANAGER_H
#define BUDDY_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "storage/simple-manager.h"

#include "exports.h"

class Account;
class XmlConfigFile;

class KADUAPI BuddyManager : public QObject, public SimpleManager<Buddy>
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyManager)

	static BuddyManager * Instance;

	BuddyManager();
	virtual ~BuddyManager();

	void init();

	void importConfiguration(XmlConfigFile *configurationStorage);

	QString mergeValue(const QString &destination, const QString &source) const;

private slots:
	void buddyDataUpdated();
	void buddySubscriptionChanged();

	void buddyContactAboutToBeAdded(const Contact &contact);
	void buddyContactAdded(const Contact &contact);
	void buddyContactAboutToBeRemoved(const Contact &contact);
	void buddyContactRemoved(const Contact &contact);

protected:
	virtual void load();

	virtual void itemAboutToBeAdded(Buddy buddy);
	virtual void itemAdded(Buddy buddy);
	virtual void itemAboutToBeRemoved(Buddy buddy);
	virtual void itemRemoved(Buddy buddy);

public:
	static BuddyManager * instance();

	virtual QString storageNodeName() { return QLatin1String("Buddies"); }
	virtual QString storageNodeItemName() { return QLatin1String("Buddy"); }

	BuddyList buddies(Account account, bool includeAnonymous = false);
	void mergeBuddies(Buddy destination, Buddy source);

	Buddy byDisplay(const QString &display, NotFoundAction action);
	Buddy byId(Account account, const QString &id, NotFoundAction action);
	Buddy byContact(Contact contact, NotFoundAction action);
	Buddy byUuid(const QUuid &uuid);

	void removeBuddyIfEmpty(Buddy buddy, bool checkOnlyForContacts = false);
	void clearOwnerAndRemoveEmptyBuddy(Contact contact, bool checkBuddyOnlyForOtherContacts = false);

signals:
	void buddyAboutToBeAdded(const Buddy &buddy);
	void buddyAdded(const Buddy &buddy);
	void buddyAboutToBeRemoved(const Buddy &buddy);
	void buddyRemoved(const Buddy &buddy);

	void buddyContactAboutToBeAdded(const Buddy &buddy, const Contact &contact);
	void buddyContactAdded(const Buddy &buddy, const Contact &contact);
	void buddyContactAboutToBeRemoved(const Buddy &buddy, const Contact &contact);
	void buddyContactRemoved(const Buddy &buddy, const Contact &contact);

	void buddyUpdated(const Buddy &buddy);
	void buddySubscriptionChanged(const Buddy &buddy);

};

#endif // BUDDY_MANAGER_H
