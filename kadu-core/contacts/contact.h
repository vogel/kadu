/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_H
#define CONTACT_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QHash>
#include <QtCore/QMetaType>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "contact-data.h"

#define PropertyRead(type, name, capitalized_name, default) \
	type name() const\
	{\
		return isNull()\
			? default\
			: Data->name();\
	}
#define PropertyWrite(type, name, capitalized_name, default) \
	void set##capitalized_name(const type &name)\
	{\
		if (!isNull())\
			Data->set##capitalized_name(name);\
	}
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

class ContactAccountData;
class XmlConfigFile;

class Contact : public QObject
{
public:
	enum ContactType
	{
		TypeNull,
		TypeAnonymous,
		TypeNormal
	};

private:
	Q_OBJECT

	QExplicitlySharedDataPointer<ContactData> Data;
	ContactType Type;

	Contact(ContactData *contactData);

	void checkNull();

public:
	static Contact loadFromStorage(StoragePoint *contactStoragePoint);

	Contact();
	Contact(ContactType type);
	Contact(const Contact &copy);
	virtual ~Contact();

	static Contact null;

	bool isNull() const { return TypeNull == Type || 0 == Data; }
	bool isAnonymous() const { return TypeNull == TypeAnonymous; }

	Contact & operator = (const Contact &copy);
	bool operator == (const Contact &compare) const;
	bool operator != (const Contact &compare) const;
	int operator < (const Contact &compare) const;

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration();
	void storeConfiguration();

	StoragePoint * storagePointForAccountData(Account *account) const;
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false) const;

	QUuid uuid() const;
	QMap<QString, QString> & customData();

	Account * prefferedAccount();
	QList<Account *> accounts();

	void addAccountData(ContactAccountData *accountData);
	ContactAccountData * accountData(Account *account) const;
	bool hasStoredAccountData(Account *account) const;
	bool hasAccountData(Account *account) const;

template<class T>
	T * moduleData(bool create = false, bool cache = false) const
	{
		return isNull() ? 0 : Data->moduleData<T>(create, cache);
	}

	QString id(Account *account) const;
	// properties

	bool isIgnored() const;
	bool setIgnored(bool ignored = true);
	bool isBlocked(Account *account) const;
	bool isOfflineTo(Account *account) const;
	void setOfflineTo(Account *account, bool offlineTo) const;
	bool isInGroup(Group *group) const;

	QString display() const;

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
	Property(unsigned short, birthYear, BirthYear, 0)
	Property(ContactData::ContactGender, gender, Gender, ContactData::GenderUnknown)
	Property(QList<Group *>, groups, Groups, QList<Group *>());

};

Q_DECLARE_METATYPE(Contact)

uint qHash(const Contact &contact);

#undef Property

#endif // CONTACT_H
