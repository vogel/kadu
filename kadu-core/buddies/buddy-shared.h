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

#include "shared/shared.h"

#include "exports.h"

class Contact;
class Group;
class XmlConfigFile;

class KADUAPI BuddyShared : public QObject, public Shared
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
	QMap<QString, QString> CustomData;
	QMap<Account, Contact> Contacts;

	BuddyType Type;

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

private slots:
	void accountContactDataIdChanged(const QString &id);

protected:
	virtual void emitUpdated();

public:
	static BuddyShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit BuddyShared(BuddyType type, QUuid uuid = QUuid());
	virtual ~BuddyShared();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	virtual void load();
	virtual void store();

	QString id(Account account);

	Account prefferedAccount();
	QList<Account> accounts();

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
	bool showInAllGroup();
	bool isInGroup(Group *group);
	void addToGroup(Group *group);
	void removeFromGroup(Group *group);

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
	KaduShared_Property(QList<Group *>, groups, Groups)
	KaduShared_PropertyBool(Blocked)
	KaduShared_PropertyBool(OfflineTo)

signals:
	void contactAboutToBeAdded(Account account);
	void contactAdded(Account account);
	void contactAboutToBeRemoved(Account account);
	void contactRemoved(Account account);
	void contactIdChanged(Account account, const QString &oldId);

	void updated();

};

// for MOC
#include "accounts/account.h"
#include "contacts/contact.h"

#endif // BUDDY_SHARED_DATA
