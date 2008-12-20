/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_ACCOUNT_DATA
#define CONTACT_ACCOUNT_DATA

#include <QtXml/QDomElement>

class Account;
class XmlConfigFile;

class ContactAccountData
{
	Account *ContactAccount;
	QString Id;

public:
	ContactAccountData();
	ContactAccountData(Account *account, const QString &id);

	virtual void loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	Account * account() { return ContactAccount; }
	QString id() { return Id; }

	bool hasFeature() { return false; }

};

#endif
