/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_SHARED_DATA
#define BUDDY_SHARED_DATA

#include <QtCore/QMap>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "configuration/uuid-storable-object.h"

#include "exports.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }

class Contact;
class Group;
class XmlConfigFile;

class KADUAPI BuddyShared : public QObject, public QSharedData, public UuidStorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddyShared)

public:
	enum BuddyGender
	{
		GenderUnknown,
		GenderMale,
		GenderFemale
	};

	enum BuddyType
	{
		TypeNull = 0,
		TypeAnonymous = 1,
		TypeNormal = 2
	};

private:
	QUuid Uuid;
	QMap<QString, QString> CustomData;
	QMap<Account, Contact> Contacts;

	BuddyType Type;

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
	QString Website;
	unsigned short BirthYear;
	BuddyGender Gender;
	QList<Group *> Groups;

	bool Ignored;
	bool Blocked;
	bool OfflineTo;

	void dataUpdated();
	void emitUpdated();

private slots:
	void accountContactDataIdChanged(const QString &id);

public:
	static BuddyShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit BuddyShared(BuddyType type, QUuid uuid = QUuid());
	virtual ~BuddyShared();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	QString id(Account account);

	Account prefferedAccount();
	QList<Account> accounts();

	void blockUpdatedSignal();
	void unblockUpdatedSignal();

	QMap<QString, QString> & customData() { return CustomData; }

	void addContact(Contact contact);
	void removeContact(Contact contact);
	void removeContact(Account account);
	Contact contact(Account account);
	QList<Contact> contacts();

	//contact type
	bool isNull() const { return TypeNull == Type; }
	bool isAnonymous() const { return TypeAnonymous == Type; }

	void setType(BuddyType type) { Type = type; }

	// properties
	bool isIgnored();
	bool setIgnored(bool ignored = true);
	bool isBlocked(Account account);
	bool isOfflineTo(Account account);
	bool setOfflineTo(Account account, bool offlineTo);
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
	Property(QString, website, Website)
	Property(unsigned short, birthYear, BirthYear)
	Property(BuddyGender, gender, Gender)
	Property(QList<Group *>, groups, Groups)

signals:
	void contactAboutToBeAdded(Account account);
	void contactAdded(Account account);
	void contactAboutToBeRemoved(Account account);
	void contactRemoved(Account account);
	void contactIdChanged(Account account, const QString &oldId);

	void updated();

};

#undef Property

// for MOC
#include "accounts/account.h"
#include "contacts/contact.h"

#endif // BUDDY_SHARED_DATA
