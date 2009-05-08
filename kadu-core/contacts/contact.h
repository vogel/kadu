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
#include "exports.h"

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

class KADUAPI Contact : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<ContactData> Data;

	Contact(ContactData *contactData);

	void checkNull();

public:
	static Contact loadFromStorage(StoragePoint *contactStoragePoint);

	Contact();
	Contact(ContactData::ContactType type);
	Contact(const Contact &copy);
	virtual ~Contact();

	static Contact null;

	bool isNull() const { return 0 == Data || Data->isNull(); }
	bool isAnonymous() const { return 0 != Data && Data->isAnonymous(); }

	Contact & operator = (const Contact &copy);
	bool operator == (const Contact &compare) const;
	bool operator != (const Contact &compare) const;
	int operator < (const Contact &compare) const;

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration();
	void store();

	StoragePoint * storagePointForAccountData(Account *account) const;
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false) const;
	void removeFromStorage();

	QUuid uuid() const;
	QMap<QString, QString> & customData();

	Account * prefferedAccount() const;
	QList<Account *> accounts() const;

	ContactData * data() const { return Data.data(); }

	void addAccountData(ContactAccountData *accountData);
	void removeAccountData(Account *account) const;
	ContactAccountData * accountData(Account *account) const;
	QList<ContactAccountData *> accountDatas() const;
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
	bool showInAllGroup() const;
	bool isInGroup(Group *group) const;
	void addToGroup(Group *group);
	void removeFromGroup(Group *group);

	QString display() const;
	void setType(ContactData::ContactType type) { Data->setType(type); }

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
