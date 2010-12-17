/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef BUDDY_SHARED_DATA
#define BUDDY_SHARED_DATA

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "avatars/avatar.h"
#include "buddies/buddy-gender.h"
#include "buddies/group.h"
#include "storage/shared.h"

#include "exports.h"

#define BuddyShared_PropertySubscriptionRead(capitalized_name) \
	bool is##capitalized_name() { ensureLoaded(); return capitalized_name; }

#define BuddyShared_PropertySubscriptionWrite(capitalized_name) \
	void set##capitalized_name(bool name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name;  buddySubscriptionChanged(); dataUpdated(); } }

#define BuddyShared_PropertySubscription(capitalized_name) \
	BuddyShared_PropertySubscriptionRead(capitalized_name) \
	BuddyShared_PropertySubscriptionWrite(capitalized_name)

class Account;
class Contact;
class XmlConfigFile;

class KADUAPI BuddyShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyShared)

	QMap<QString, QString> CustomData;
	QList<Contact> Contacts;

	Avatar BuddyAvatar;
	QString Display;
	QString FirstName;
	QString LastName;
	QString FamilyName;
	QString City;
	QString FamilyCity;
	QString NickName;
	QString HomePhone;
	QString Mobile;
	QString Email;
	QString Website;
	unsigned short BirthYear;
	BuddyGender Gender;
	QList<Group> Groups;

	bool Anonymous;
	bool Blocked;
	bool OfflineTo;

protected:
	virtual void load();
	virtual void emitUpdated();

public:
	static BuddyShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint);
	static BuddyShared * loadFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint);

	explicit BuddyShared(QUuid uuid = QUuid());
	virtual ~BuddyShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void importConfiguration(const QDomElement &parent);

	virtual void store();
    virtual bool shouldStore();
	virtual void aboutToBeRemoved();

	QString id(Account account);

	QMap<QString, QString> & customData() { return CustomData; }

	void addContact(Contact contact);
	void removeContact(Contact contact);
	QList<Contact> contacts(Account account);
	QList<Contact> contacts();

	void sortContacts();
	void normalizePriorities();

	// properties
	bool showInAllGroup();
	bool isInGroup(Group group);
	void addToGroup(Group group);
	void removeFromGroup(Group group);

	bool isEmpty() const;

	KaduShared_Property(Avatar, buddyAvatar, BuddyAvatar)
	KaduShared_Property(QString, display, Display)
	KaduShared_Property(QString, firstName, FirstName)
	KaduShared_Property(QString, lastName, LastName)
	KaduShared_Property(QString, familyName, FamilyName)
	KaduShared_Property(QString, city, City)
	KaduShared_Property(QString, familyCity, FamilyCity)
	KaduShared_Property(QString, nickName, NickName)
	KaduShared_Property(QString, homePhone, HomePhone)
	KaduShared_Property(QString, mobile, Mobile)
	KaduShared_Property(QString, email, Email)
	KaduShared_Property(QString, website, Website)
	KaduShared_Property(unsigned short, birthYear, BirthYear)
	KaduShared_Property(BuddyGender, gender, Gender)
	KaduShared_Property(QList<Group>, groups, Groups)
	BuddyShared_PropertySubscription(Anonymous)
	BuddyShared_PropertySubscription(Blocked)
	BuddyShared_PropertySubscription(OfflineTo)

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	void updated();
	void buddySubscriptionChanged();
};

// for MOC
#include "accounts/account.h"
#include "contacts/contact.h"

#endif // BUDDY_SHARED_DATA
