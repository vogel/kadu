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
	Q_OBJECT

	QExplicitlySharedDataPointer<ContactData> Data;

public:
	Contact();
	Contact(const Contact &copy);
	virtual ~Contact();

	Contact & operator = (const Contact &copy);

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	QUuid uuid() { return Data->uuid(); };
	QMap<QString, QString> & customData() { return Data->customData(); }

	void addAccountData(ContactAccountData *accountData) { Data->addAccountData(accountData); }

};

#endif // CONTACT_H
