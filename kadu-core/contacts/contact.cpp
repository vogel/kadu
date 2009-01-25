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

Contact Contact::null(Contact::TypeNull);

Contact::Contact()
	: Data(new ContactData()), Type(Contact::TypeNormal)
{
}

Contact::Contact(StoragePoint *contactStoragePoint)
	: Data(new ContactData(contactStoragePoint)), Type(Contact::TypeNormal)
{
}

Contact::Contact(Contact::ContactType type)
	: Type(type), Data(Contact::TypeNull != type ? new ContactData() : 0)
{
}

Contact::Contact(const Contact &copy)
	: Data(copy.Data), Type(copy.Type)
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

bool Contact::operator != (const Contact& compare) const
{
	return Data != compare.Data;
}

int Contact::operator < (const Contact& compare) const
{
	return Data.data() - compare.Data.data();
}

void Contact::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->importConfiguration(configurationStorage, parent);
}

void Contact::loadConfiguration()
{
	Data->loadConfiguration();
}

void Contact::storeConfiguration()
{
	Data->storeConfiguration();
}

StoragePoint * Contact::storagePointForAccountData(Account* account, bool create) const
{
	return Data->storagePointForAccountData(account, create);
}

StoragePoint * Contact::storagePointForModuleData(const QString& module, bool create) const
{
	return Data->storagePointForModuleData(module, create);
}

QUuid Contact::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
};

QMap<QString, QString> & Contact::customData()
{
	checkNull();
	return Data->customData();
}

Account * Contact::prefferedAccount()
{
	checkNull();
	return Data->prefferedAccount();
}

void Contact::addAccountData(ContactAccountData *accountData)
{
	checkNull();
	Data->addAccountData(accountData);
}

ContactAccountData * Contact::accountData(Account *account) const
{
	return isNull() ? 0 : Data->accountData(account);
}

QString Contact::id(Account *account) const
{
	return isNull()
		? QString::null
		: Data->id(account);
}

uint qHash(const Contact &contact)
{
	return qHash(contact.uuid().toString());
}

bool Contact::isBlocked(Account *account) const
{
	return isNull()
		? false
		: Data->isBlocked(account);
}

bool Contact::isOfflineTo(Account *account) const
{
	return isNull()
		? false
		: Data->isOfflineTo(account);
}
