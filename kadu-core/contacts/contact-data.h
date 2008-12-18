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

class ContactAccountData;
class XmlConfigFile;

class ContactData : public QSharedData
{
	QUuid Uuid;
	QMap<QString, QString> CustomData;
	QMap<QString, ContactAccountData *> AccountsData;

public:
	ContactData(QUuid uniqueId = QUuid());
	~ContactData();

	QUuid uuid() { return Uuid; }

	QMap<QString, QString> & customData() { return CustomData; }

	void addAccountData(ContactAccountData *accountData);

	void importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

};

#endif // CONTACT_DATA
