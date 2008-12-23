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
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtXml/QDomElement>

#include "contact-data.h"

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

	void checkNull();

public:
	Contact();
	Contact(ContactType type);
	Contact(const Contact &copy);
	virtual ~Contact();

	static Contact null;

	bool isNull() const { return TypeNull == Type; }
	bool isAnonymous() const { return TypeNull == TypeAnonymous; }

	Contact & operator = (const Contact &copy);
	bool operator == (const Contact &compare) const;
	bool operator != (const Contact &compare) const;
	int operator < (const Contact &compare) const;

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QUuid uuid();
	QMap<QString, QString> & customData();

	void addAccountData(ContactAccountData *accountData);
	ContactAccountData * accountData(Account *account) const;

	QString id(Account *account) const;

};

#endif // CONTACT_H
