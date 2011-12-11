/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "buddies/buddy-gender.h"
#include "storage/shared.h"

#include "exports.h"

#define BuddyShared_PropertyDirtyWrite(type, name, capitalized_name) \
	void set##capitalized_name(type name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name; dataUpdated(); markContactsDirty(); } }

#define BuddyShared_PropertyDirty(type, name, capitalized_name) \
	KaduShared_PropertyRead(type, name, capitalized_name) \
	BuddyShared_PropertyDirtyWrite(type, name, capitalized_name)

#define BuddyShared_PropertySubscriptionRead(capitalized_name) \
	bool is##capitalized_name() { ensureLoaded(); return capitalized_name; }

#define BuddyShared_PropertySubscriptionWrite(capitalized_name) \
	void set##capitalized_name(bool name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name;  buddySubscriptionChanged(); dataUpdated(); } }

#define BuddyShared_PropertySubscriptionDirtyWrite(capitalized_name) \
	void set##capitalized_name(bool name) { ensureLoaded(); if (capitalized_name != name) { capitalized_name = name;  buddySubscriptionChanged(); dataUpdated(); markContactsDirty(); } }

#define BuddyShared_PropertySubscription(capitalized_name) \
	BuddyShared_PropertySubscriptionRead(capitalized_name) \
	BuddyShared_PropertySubscriptionWrite(capitalized_name)

#define BuddyShared_PropertySubscriptionDirty(capitalized_name) \
	BuddyShared_PropertySubscriptionRead(capitalized_name) \
	BuddyShared_PropertySubscriptionDirtyWrite(capitalized_name)

class Account;
class Avatar;
class Contact;
class Group;
class XmlConfigFile;

class KADUAPI BuddyShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyShared)

	bool CollectingGarbage;

	QMap<QString, QString> CustomData;
	QList<Contact> Contacts;

	Avatar *BuddyAvatar;
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
	bool PreferHigherStatuses;

	bool Anonymous;
	bool Blocked;
	bool OfflineTo;

	bool doAddToGroup(const Group &group);
	bool doRemoveFromGroup(const Group &group);

private slots:
	void avatarUpdated();
	void groupAboutToBeRemoved();
	void markContactsDirty();

protected:
	virtual QSharedPointer<StoragePoint> createStoragePoint();

	virtual void load();
	virtual void store();
	virtual bool shouldStore();
	virtual void emitUpdated();

public:
	static BuddyShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint);
	static BuddyShared * loadFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint);

	explicit BuddyShared(const QUuid &uuid = QUuid());
	virtual ~BuddyShared();

	void collectGarbage();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void importConfiguration(const QDomElement &parent);
	void importConfiguration(); // imports configuration from custom data values

	virtual void aboutToBeRemoved();

	QString id(const Account &account);

	QMap<QString, QString> & customData() { return CustomData; }

	void addContact(const Contact &contact);
	void removeContact(const Contact &contact);
	QVector<Contact> contacts(const Account &account);
	const QList<Contact> & contacts();

	void sortContacts();
	void normalizePriorities();

	bool isEmpty(bool checkOnlyForContacts);

	KaduShared_PropertyRead(const QList<Group> &, groups, Groups)
	void setGroups(const QList<Group> &groups);
	bool showInAllGroup();
	bool isInGroup(const Group &group);
	void addToGroup(const Group &group);
	void removeFromGroup(const Group &group);

	KaduShared_PropertyReadDecl(Avatar, buddyAvatar)
	void setBuddyAvatar(const Avatar &buddyAvatar);

	KaduShared_PropertyRead(const QString &, display, Display)
	void setDisplay(const QString &display);

	BuddyShared_PropertyDirty(const QString &, firstName, FirstName)
	BuddyShared_PropertyDirty(const QString &, lastName, LastName)
	BuddyShared_PropertyDirty(const QString &, familyName, FamilyName)
	BuddyShared_PropertyDirty(const QString &, city, City)
	BuddyShared_PropertyDirty(const QString &, familyCity, FamilyCity)
	BuddyShared_PropertyDirty(const QString &, nickName, NickName)
	BuddyShared_PropertyDirty(const QString &, homePhone, HomePhone)
	BuddyShared_PropertyDirty(const QString &, mobile, Mobile)
	BuddyShared_PropertyDirty(const QString &, email, Email)
	BuddyShared_PropertyDirty(const QString &, website, Website)
	KaduShared_Property(unsigned short, birthYear, BirthYear)
	KaduShared_Property(BuddyGender, gender, Gender)
	KaduShared_Property(bool, preferHigherStatuses, PreferHigherStatuses)
	BuddyShared_PropertySubscription(Anonymous)
	BuddyShared_PropertySubscriptionDirty(Blocked)
	BuddyShared_PropertySubscriptionDirty(OfflineTo)

	quint16 unreadMessagesCount();

signals:
	void contactAboutToBeAdded(const Contact &contact);
	void contactAdded(const Contact &contact);
	void contactAboutToBeRemoved(const Contact &contact);
	void contactRemoved(const Contact &contact);

	void updated();
	void displayUpdated();
	void buddySubscriptionChanged();

};

// for MOC
#include "contacts/contact.h"

#endif // BUDDY_SHARED_DATA
