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
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "buddies/group.h"
#include "storage/shared.h"

#include "exports.h"

class Contact;
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

private:
	QMap<QString, QString> CustomData;
	QList<Contact> Contacts;

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

	bool Anonymous;
	bool Ignored;
	bool Blocked;
	bool OfflineTo;

private slots:
	void accountContactDataIdChanged(const QString &id);

protected:
	virtual void load();
	virtual void emitUpdated();

public:
	static BuddyShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit BuddyShared(QUuid uuid = QUuid());
	virtual ~BuddyShared();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	virtual void store();

	QString id(Account account);

	Account prefferedAccount();
	QList<Account> accounts();

	QMap<QString, QString> & customData() { return CustomData; }

	void addContact(Contact contact);
	void removeContact(Contact contact);
	QList<Contact> contacts(Account account);
	QList<Contact> contacts();
	Contact prefferedContact();


	// properties
	bool showInAllGroup();
	bool isInGroup(Group group);
	void addToGroup(Group group);
	void removeFromGroup(Group group);

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
	KaduShared_Property(QList<Group>, groups, Groups)
	KaduShared_PropertyBool(Anonymous)
	KaduShared_PropertyBool(Ignored)
	KaduShared_PropertyBool(Blocked)
	KaduShared_PropertyBool(OfflineTo)

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);
	void contactIdChanged(Contact contact, const QString &oldId);

	void updated();

};

// for MOC
#include "accounts/account.h"
#include "contacts/contact.h"

#endif // BUDDY_SHARED_DATA
