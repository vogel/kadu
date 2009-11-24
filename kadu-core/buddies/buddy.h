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
#include <QtCore/QHash>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "buddies/buddy-shared.h"
#include "shared/shared-base.h"
#include "exports.h"

class Account;
class BuddyShared;
class Contact;
class StoragePoint;
class XmlConfigFile;

class KADUAPI Buddy : public QObject, public SharedBase<BuddyShared>
{
	Q_OBJECT

	explicit Buddy(bool null);

public:
	static Buddy loadFromStorage(StoragePoint *contactStoragePoint);
	static Buddy null;

	static Buddy dummy();

	Buddy();
	Buddy(BuddyShared *data);
	Buddy(QObject *data);
	Buddy(const Buddy &copy);
	virtual ~Buddy();

	Buddy & operator = (const Buddy &copy)
	{
		clone(copy);
		return *this;
	}

	void mergeWith(Buddy buddy); // TODO: 0.8 refactor

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void store();

	StoragePoint * storagePointForModuleData(const QString &module, bool create = false) const;

	QString customData(const QString &key);
	void setCustomData(const QString &key, const QString &value);
	void removeCustomData(const QString &key);

	Account prefferedAccount() const;
	QList<Account> accounts() const;

// 	void setData(BuddyShared *data) { Data = data; }  // TODO: 0.8 tricky merge, this should work well ;)

	void addContact(Contact contact);
	void removeContact(Contact contact) const;
	void removeContact(Account account) const;
	Contact contact(Account account) const;
	QList<Contact> contacts() const;
	bool hasContact(Account account) const;

template<class T>
	T * moduleData(bool create = false, bool cache = false) const
	{
		return isNull() ? 0 : data()->moduleData<T>(create, cache);
	}

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
	KaduSharedBase_Property(BuddyShared::BuddyGender, gender, Gender)
	KaduSharedBase_Property(QList<Group>, groups, Groups)
	KaduSharedBase_PropertyBool(Anonymous)
	KaduSharedBase_PropertyBool(Ignored)
	KaduSharedBase_PropertyBool(Blocked)
	KaduSharedBase_PropertyBool(OfflineTo)

};

Q_DECLARE_METATYPE(Buddy)

#endif // BUDDY_H
