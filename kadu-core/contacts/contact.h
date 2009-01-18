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

#define Property(type, name, capitalized_name, default) \
	type name() const\
	{\
		return isNull()\
			? default\
			: Data->name();\
	} \
	void set##capitalized_name(const type &name)\
	{\
		if (!isNull())\
			Data->set##capitalized_name(name);\
	}

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

	bool isNull() const { return TypeNull == Type || 0 == Data; }
	bool isAnonymous() const { return TypeNull == TypeAnonymous; }

	Contact & operator = (const Contact &copy);
	bool operator == (const Contact &compare) const;
	bool operator != (const Contact &compare) const;
	int operator < (const Contact &compare) const;

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QUuid uuid() const;
	QMap<QString, QString> & customData();

	void addAccountData(ContactAccountData *accountData);
	ContactAccountData * accountData(Account *account) const;

	QString id(Account *account) const;
	// properties

	bool isBlocked(Account *account) const;
	bool isOfflineTo(Account *account) const;

	Property(QString, display, Display, QString::null)
	Property(QString, firstName, FirstName, QString::null)
	Property(QString, lastName, LastName, QString::null)
	Property(QString, nickName, NickName, QString::null)
	Property(QString, homePhone, HomePhone, QString::null)
	Property(QString, mobile, Mobile, QString::null)
	Property(QString, email, Email, QString::null)

};

Q_DECLARE_METATYPE(Contact)

uint qHash(const Contact &contact);

#undef Property

#endif // CONTACT_H
