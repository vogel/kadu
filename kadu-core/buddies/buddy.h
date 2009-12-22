/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_H
#define BUDDY_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "buddies/buddy-gender.h"
#include "storage/shared-base.h"
#include "exports.h"

class Account;
class BuddyShared;
class Contact;
class ContactShared;
class Group;
class StoragePoint;
class XmlConfigFile;

class KADUAPI Buddy : public QObject, public SharedBase<BuddyShared>
{
	Q_OBJECT
	KaduSharedBaseClass(Buddy)

	friend class ContactShared;
	// only allow ContactShared to access these methods
	void addContact(Contact contact);
	void removeContact(Contact contact) const;

public:
	static Buddy create();
	static Buddy loadFromStorage(StoragePoint *contactStoragePoint);
	static Buddy null;

	static Buddy dummy();

	Buddy();
	Buddy(BuddyShared *data);
	Buddy(QObject *data);
	Buddy(const Buddy &copy);
	virtual ~Buddy();

	void mergeWith(Buddy buddy); // TODO: 0.8 refactor

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void store();

	StoragePoint * storagePointForModuleData(const QString &module, bool create = false) const;

	QString customData(const QString &key);
	void setCustomData(const QString &key, const QString &value);
	void removeCustomData(const QString &key);

	Account prefferedAccount() const;
	Contact prefferedContact() const;

// 	void setData(BuddyShared *data) { Data = data; }  // TODO: 0.8 tricky merge, this should work well ;)

	QList<Contact> contacts(Account account) const;
	QList<Contact> contacts() const;
	bool hasContact(Account account) const;

	QString id(Account account) const;
	// properties

	bool showInAllGroup() const;
	bool isInGroup(Group group) const;
	void addToGroup(Group group);
	void removeFromGroup(Group group);

	QString display() const;

	KaduSharedBase_PropertyWrite(QString, display, Display)
	KaduSharedBase_Property(QString, firstName, FirstName)
	KaduSharedBase_Property(QString, lastName, LastName)
	KaduSharedBase_Property(QString, familyName, FamilyName)
	KaduSharedBase_Property(QString, city, City)
	KaduSharedBase_Property(QString, familyCity, FamilyCity)
	KaduSharedBase_Property(QString, nickName, NickName)
	KaduSharedBase_Property(QString, homePhone, HomePhone)
	KaduSharedBase_Property(QString, mobile, Mobile)
	KaduSharedBase_Property(QString, email, Email)
	KaduSharedBase_Property(QString, website, Website)
	KaduSharedBase_Property(unsigned short, birthYear, BirthYear)
	KaduSharedBase_Property(BuddyGender, gender, Gender)
	KaduSharedBase_Property(QList<Group>, groups, Groups)
	KaduSharedBase_PropertyBool(Anonymous)
	KaduSharedBase_PropertyBool(Ignored)
	KaduSharedBase_PropertyBool(Blocked)
	KaduSharedBase_PropertyBool(OfflineTo)

};

Q_DECLARE_METATYPE(Buddy)

// for MOC
#include "buddies/group.h"

#endif // BUDDY_H
