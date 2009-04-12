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
	void set##capitalized_name(const type &name) { capitalized_name = name; emitUpdated(); }

class Account;
class ContactAccountData;
class Group;
class XmlConfigFile;

class KADUAPI ContactData : public QObject, public QSharedData, public UuidStorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactData)

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

	int BlockUpdatedSignalCount;
	bool Updated;

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

	void dataUpdated();
	void emitUpdated();

public:
	static ContactData * loadFromStorage(StoragePoint *contactStoragePoint);

	ContactData(QUuid uuid = QUuid());
	~ContactData();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	virtual void load();
	virtual void store();

	StoragePoint * storagePointForAccountData(Account *account);

	virtual QUuid uuid() const { return Uuid; }
	QString id(Account *account);

	Account * prefferedAccount();
	QList<Account *> accounts();

	void blockUpdatedSignal();
	void unblockUpdatedSignal();

	QMap<QString, QString> & customData() { return CustomData; }

	void addAccountData(ContactAccountData *accountData);
	void removeAccountData(Account *account);
	ContactAccountData * accountData(Account *account);
	QList<ContactAccountData *> accountDatas();
	bool hasStoredAccountData(Account *account);

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

signals:
	void updated();

};

#undef Property

#endif // CONTACT_DATA
