/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xml_config_file.h"

#include "contact.h"

Contact Contact::null(true);

Contact::Contact()
	: Data(new ContactData())
{
}

Contact::Contact(bool)
	: Data(0)
{
}

Contact::Contact(const Contact &copy)
	: Data(copy.Data)
{
}

Contact::~Contact()
{
}

void Contact::checkNull()
{
	if (isNull())
		Data = new ContactData();
}

Contact & Contact::operator = (const Contact& copy)
{
	Data = copy.Data;
	return *this;
}

bool Contact::operator == (const Contact& compare) const
{
	return Data == compare.Data;
}

int Contact::operator < (const Contact& compare) const
{
	return Data.data() - compare.Data.data();
}

void Contact::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->importConfiguration(configurationStorage, parent);
}

void Contact::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->loadConfiguration(configurationStorage, parent);
}

void Contact::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->storeConfiguration(configurationStorage, parent);
}

QUuid Contact::uuid()
{
	checkNull();
	return Data->uuid();
};

QMap<QString, QString> & Contact::customData()
{
	checkNull();
	return Data->customData();
}

void Contact::addAccountData(ContactAccountData *accountData)
{
	checkNull();
	Data->addAccountData(accountData);
}

ContactAccountData * Contact::accountData(Account *account)
{
	checkNull();
	return Data->accountData(account);
}

QString Contact::id(Account *account)
{
	checkNull();
	return Data->id(account);
}
