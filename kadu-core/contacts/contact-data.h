/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_DATA
#define CONTACT_DATA

#include <QtCore/QMap>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "configuration/uuid-storable-object.h"

#include "exports.h"

#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class Account;
class ContactAccountData;
class ContactModuleData;
class Group;
class XmlConfigFile;

class KADUAPI ContactData : public QSharedData, public UuidStorableObject
{
public:
	enum ContactGender
	{
		GenderUnknown,
		GenderMale,
		GenderFemale
	};

private:
	QUuid Uuid;
	QMap<QString, QString> CustomData;
	QMap<Account *, ContactAccountData *> AccountsData;
	QMap<QString, ContactModuleData *> ModulesData;

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
	unsigned short BirthYear;
	ContactGender Gender;
	QList<Group *> Groups;

	bool Ignored;
	bool Blocked;
	bool OfflineTo;

public:
	static ContactData * loadFromStorage(StoragePoint *contactStoragePoint);

	ContactData(QUuid uuid = QUuid());
	~ContactData();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration();
	void storeConfiguration();

	StoragePoint * storagePointForAccountData(Account *account);
	StoragePoint * storagePointForModuleData(const QString &module, bool create = false);

	virtual QUuid uuid() { return Uuid; }
	QString id(Account *account);

	Account * prefferedAccount();
	QList<Account *> accounts();

	QMap<QString, QString> & customData() { return CustomData; }

	void addAccountData(ContactAccountData *accountData);
	void removeAccountData(Account *account);
	ContactAccountData * accountData(Account *account);
	QList<ContactAccountData *> accountDatas();
	bool hasStoredAccountData(Account *account);

	ContactModuleData * moduleData(const QString &key);

	// TODO: 0.6.6 - check create and cache implementation
template<class T>
	T * moduleData(bool create = false, bool cache = false)
	{
		T *result = 0;
		if (!cache)
		{
			result = new T(storagePointForModuleData(T::key(), create));
			result->loadFromStorage();
		}
		else
		{
			if (!ModulesData.contains(T::key()))
			{
				result = new T(storagePointForModuleData(T::key(), create));
				ModulesData[T::key()] = result;
				result->loadFromStorage();
			}
			else
				result = dynamic_cast<T *>(ModulesData[T::key()]);

		}
		return result;
	}

	// properties
	bool isIgnored();
	bool setIgnored(bool ignored = true);
	bool isBlocked(Account *account);
	bool isOfflineTo(Account *account);
	bool setOfflineTo(Account *account, bool offlineTo);
	bool showInAllGroup();
	bool isInGroup(Group *group);
	void addToGroup(Group *group);
	void removeFromGroup(Group *group);

	Property(QString, display, Display)
	Property(QString, firstName, FirstName)
	Property(QString, lastName, LastName)
	Property(QString, familyName, FamilyName)
	Property(QString, city, City)
	Property(QString, familyCity, FamilyCity)
	Property(QString, nickName, NickName)
	Property(QString, homePhone, HomePhone)
	Property(QString, mobile, Mobile)
	Property(QString, email, Email)
	Property(unsigned short, birthYear, BirthYear)
	Property(ContactGender, gender, Gender)
	Property(QList<Group *>, groups, Groups);

};

#undef Property

#endif // CONTACT_DATA
