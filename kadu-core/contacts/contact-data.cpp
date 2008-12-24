/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtXml/QDomNamedNodeMap>

#include "accounts/account.h"
#include "contacts/contact-account-data.h"
#include "xml_config_file.h"

#include "contact-data.h"

ContactData::ContactData(QUuid uuid)
	: Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

ContactData::~ContactData()
{
}

void ContactData::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomNamedNodeMap attributes = parent.attributes();
	int count = attributes.count();

	for (int i = 0; i < count; i++)
	{
		QDomAttr attribute = attributes.item(i).toAttr();
		CustomData.insert(attribute.name(), attribute.value());
	}
}

void ContactData::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
}

#define Property(name) \
	configurationStorage->createTextNode(customDataValues, #name, name);

void ContactData::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement customDataValues = configurationStorage->getNode(parent,
			"CustomDataValues", XmlConfigFile::ModeCreate);

	foreach (QString key, CustomData.keys())
	{
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);
	}

	Property(Nick)
	Property(FirstName)
	Property(LastName)
	Property(NickName)
	Property(HomePhone)
	Property(Mobile)
	Property(Email)

	foreach (ContactAccountData *accountData, AccountsData.values())
	{
		QDomElement contactAccountData = configurationStorage->getUuidNode(parent,
			"ContactAccountData", accountData->account()->uuid(), XmlConfigFile::ModeCreate);
		accountData->storeConfiguration(configurationStorage, contactAccountData);
	}
}

void ContactData::addAccountData(ContactAccountData *accountData)
{
	AccountsData.insert(accountData->account(), accountData);
}

ContactAccountData * ContactData::accountData(Account *account)
{
	if (!AccountsData.contains(account))
		return 0;

	return AccountsData[account];
}

QString ContactData::id(Account *account)
{
	if (AccountsData.contains(account))
		return AccountsData[account]->id();

	return QString::null;
}

// properties

bool ContactData::isBlocked(Account *account)
{
	ContactAccountData *cad = accountData(account);
	return cad
		? cad->isBlocked()
		: Blocked;
}

bool ContactData::isOfflineTo(Account *account)
{
	ContactAccountData *cad = accountData(account);
	return cad
		? cad->isOfflineTo()
		: OfflineTo;
}
