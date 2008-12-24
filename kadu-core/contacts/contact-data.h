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
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class Account;
class ContactAccountData;
class XmlConfigFile;

class ContactData : public QSharedData
{
	QUuid Uuid;
	QMap<QString, QString> CustomData;
	QMap<Account *, ContactAccountData *> AccountsData;

	QString Nick;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString HomePhone;
	QString Mobile;
	QString Email;

	bool Blocked;
	bool OfflineTo;

public:
	ContactData(QUuid uniqueId = QUuid());
	~ContactData();

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QUuid uuid() { return Uuid; }
	QString id(Account *account);

	QMap<QString, QString> & customData() { return CustomData; }

	void addAccountData(ContactAccountData *accountData);
	ContactAccountData * accountData(Account *account);

	// properties
	bool isBlocked(Account *account);
	bool isOfflineTo(Account *account);

	Property(QString, nick, Nick)
	Property(QString, firstName, FirstName)
	Property(QString, lastName, LastName)
	Property(QString, nickName, NickName)
	Property(QString, homePhone, HomePhone)
	Property(QString, mobile, Mobile)
	Property(QString, email, Email)

};

#undef Property

#endif // CONTACT_DATA
