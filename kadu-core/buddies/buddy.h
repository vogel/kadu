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

#include "buddy-shared.h"
#include "exports.h"

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name, default) \
	type name() const\
	{\
		return isNull()\
			? default\
			: Data->name();\
	}

#undef PropertyWrite
#define PropertyWrite(type, name, capitalized_name, default) \
	void set##capitalized_name(const type &name)\
	{\
		if (!isNull())\
			Data->set##capitalized_name(name);\
	}

#undef Property
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

#undef PropertyBoolRead
#define PropertyBoolRead(capitalized_name, default) \
	bool is##capitalized_name() const;

#undef PropertyBoolWrite
#define PropertyBoolWrite(capitalized_name, default) \
	void set##capitalized_name(bool name) const;

#undef PropertyBool
#define PropertyBool(capitalized_name, default) \
	PropertyBoolRead(capitalized_name, default) \
	PropertyBoolWrite(capitalized_name, default)

class Contact;
class XmlConfigFile;

class KADUAPI Buddy : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<BuddyShared> Data;

	Buddy(BuddyShared *contactData);

	void checkNull();

public:
	static Buddy loadFromStorage(StoragePoint *contactStoragePoint);

	explicit Buddy(BuddyShared::BuddyType type = BuddyShared::TypeNormal);
	Buddy(const Buddy &copy);
	virtual ~Buddy();

	static Buddy null;

	static Buddy dummy();

	bool isNull() const { return 0 == Data || Data->isNull(); }
	bool isAnonymous() const { return 0 != Data && Data->isAnonymous(); }

	void mergeWith(Buddy buddy); // TODO: 0.8 refactor

	Buddy & operator = (const Buddy &copy);
	bool operator == (const Buddy &compare) const;
	bool operator != (const Buddy &compare) const;
	int operator < (const Buddy &compare) const;

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration();
	void store();

	StoragePoint * storagePointForModuleData(const QString &module, bool create = false) const;
	void removeFromStorage();

	QUuid uuid() const;
	QMap<QString, QString> & customData();

	Account prefferedAccount() const;
	QList<Account> accounts() const;

	BuddyShared * data() const { return Data.data(); }
	void setData(BuddyShared *data) { Data = data; }  // TODO: 0.8 tricky merge, this should work well ;)

	void addContact(Contact contact);
	void removeContact(Contact contact) const;
	void removeContact(Account account) const;
	Contact contact(Account account) const;
	QList<Contact> contacts() const;
	bool hasContact(Account account) const;

template<class T>
	T * moduleData(bool create = false, bool cache = false) const
	{
		return isNull() ? 0 : Data->moduleData<T>(create, cache);
	}

	QString id(Account account) const;
	// properties

	bool isIgnored() const;
	bool setIgnored(bool ignored = true);
	bool showInAllGroup() const;
	bool isInGroup(Group *group) const;
	void addToGroup(Group *group);
	void removeFromGroup(Group *group);

	QString display() const;
	void setType(BuddyShared::BuddyType type) { Data->setType(type); }

	PropertyWrite(QString, display, Display, QString::null)
	Property(QString, firstName, FirstName, QString::null)
	Property(QString, lastName, LastName, QString::null)
	Property(QString, familyName, FamilyName, QString::null)
	Property(QString, city, City, QString::null)
	Property(QString, familyCity, FamilyCity, QString::null)
	Property(QString, nickName, NickName, QString::null)
	Property(QString, homePhone, HomePhone, QString::null)
	Property(QString, mobile, Mobile, QString::null)
	Property(QString, email, Email, QString::null)
	Property(QString, website, Website, QString::null)
	Property(unsigned short, birthYear, BirthYear, 0)
	Property(BuddyShared::BuddyGender, gender, Gender, BuddyShared::GenderUnknown)
	Property(QList<Group *>, groups, Groups, QList<Group *>())
	PropertyBool(Blocked, blocked)
	PropertyBool(OfflineTo, offlineTo)

};

Q_DECLARE_METATYPE(Buddy)

uint qHash(const Buddy &buddy);

#undef PropertyRead
#undef PropertyWrite
#undef Property

#endif // BUDDY_H
