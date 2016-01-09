/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class Account;
class ConfigurationApi;
class Configuration;

class KADUAPI BuddyManager : public QObject, public SimpleManager<Buddy>
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyManager(QObject *parent = nullptr);
	virtual ~BuddyManager();

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

protected:
	virtual void load();

	virtual void itemAboutToBeAdded(Buddy buddy);
	virtual void itemAdded(Buddy buddy);
	virtual void itemAboutToBeRemoved(Buddy buddy);
	virtual void itemRemoved(Buddy buddy);

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

private:
	QPointer<Configuration> m_configuration;

	QString mergeValue(const QString &destination, const QString &source) const;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);

	void buddyDataUpdated();
	void buddySubscriptionChanged();

	void buddyContactAboutToBeAdded(const Contact &contact);
	void buddyContactAdded(const Contact &contact);
	void buddyContactAboutToBeRemoved(const Contact &contact);
	void buddyContactRemoved(const Contact &contact);

};
