/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QVariant>
#include <QtXml/QDomNamedNodeMap>

#include "accounts/account.h"

#include "configuration/storage-point.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-module-data.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"

#include "xml_config_file.h"

#include "contact-data.h"

ContactData::ContactData(QUuid uuid)
	: Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Ignored(false), Blocked(false), OfflineTo(false)
{
}

ContactData::~ContactData()
{
}

ContactData * ContactData::loadFromStorage(StoragePoint *contactStoragePoint)
{
	ContactData *result = new ContactData(QUuid());
	result->setStorage(contactStoragePoint);
	result->loadConfiguration();

	return result;
}

StoragePoint * ContactData::createStoragePoint() const
{
	StoragePoint *parent = ContactManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement contactNode = parent->storage()->getUuidNode(parent->point(), "Contact", Uuid.toString());
	return new StoragePoint(parent->storage(), contactNode);
}

StoragePoint * ContactData::storagePointForAccountData(Account *account)
{
	StoragePoint *parent = storage();
	if (!parent || !parent->storage())
		return 0;

	QDomElement accountDataNode = parent->storage()->getUuidNode(parent->point(), "ContactAccountData",
			account->uuid().toString(), XmlConfigFile::ModeGet);
	return accountDataNode.isNull()
		? 0
		: new StoragePoint(parent->storage(), accountDataNode);
}

StoragePoint * ContactData::storagePointForModuleData(const QString &module, bool create)
{
	StoragePoint *parent = storage();
	if (!parent || !parent->storage())
		return 0;

	QDomElement moduleDataNode = parent->storage()->getNamedNode(parent->point(), "ContactModuleData",
			module, create ? XmlConfigFile::ModeGet : XmlConfigFile::ModeFind);
	return moduleDataNode.isNull()
		? 0
		: new StoragePoint(parent->storage(), moduleDataNode);
}

#undef Property
#define Property(name, old_name) \
	set##name(CustomData[#old_name]); \
	CustomData.remove(#old_name);

void ContactData::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomNamedNodeMap attributes = parent.attributes();
	int count = attributes.count();

	for (int i = 0; i < count; i++)
	{
		QDomAttr attribute = attributes.item(i).toAttr();
		CustomData.insert(attribute.name(), attribute.value());
	}

	QStringList groups = CustomData["groups"].split(',', QString::SkipEmptyParts);
	foreach (const QString &group, groups)
		Groups << GroupManager::instance()->byName(group);
	CustomData.remove("groups");

	Property(Display, altnick)
	Property(FirstName, first_name)
	Property(LastName, last_name)
	Property(NickName, nick_name)
	Property(HomePhone, home_phone)
	Property(Mobile, mobile)
	Property(Email, email)
}

#undef Property
#define Property(name)\
	set##name(configurationStorage->getTextNode(parent, #name));

void ContactData::loadConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	Uuid = QUuid(parent.attribute("uuid"));

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues", XmlConfigFile::ModeFind);
	QDomNodeList customDataValuesList = customDataValues.elementsByTagName("CustomDataValue");

	int count = customDataValuesList.count();
	for (int i = 0; i < count; i++)
	{
		QDomNode customDataNode = customDataValuesList.at(i);
		QDomElement customDataElement = customDataNode.toElement();
		if (customDataElement.isNull())
			continue;

		QString name = customDataElement.attribute("name");
		if (!name.isEmpty())
			CustomData[name] = customDataElement.text();
	}

	Groups.clear();
	QDomElement groupsNode = configurationStorage->getNode(parent, "Groups", XmlConfigFile::ModeFind);
	if (!groupsNode.isNull())
	{
		QDomNodeList groupsList = groupsNode.elementsByTagName("Group");

		count = groupsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement groupElement = groupsList.at(i).toElement();
			if (groupElement.isNull())
				continue;
			Group *group = GroupManager::instance()->byUuid(groupElement.text());
			if (group)
				Groups << group;
		}
	}
	
	Property(Display)
	Property(FirstName)
	Property(LastName)
	Property(NickName)
	Property(HomePhone)
	Property(Mobile)
	Property(Email)

	Ignored = QVariant(configurationStorage->getTextNode(parent, "Ignored")).toBool();
}

#undef Property
#define Property(name) \
	configurationStorage->createTextNode(parent, #name, name);

void ContactData::storeConfiguration()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues");

	foreach (const QString &key, CustomData.keys())
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);

	Property(Display)
	Property(FirstName)
	Property(LastName)
	Property(NickName)
	Property(HomePhone)
	Property(Mobile)
	Property(Email)

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "Groups", XmlConfigFile::ModeCreate);
		foreach (const Group *group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group->uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "Groups");

	configurationStorage->createTextNode(parent, "Ignored", QVariant(Ignored).toString());

	foreach (ContactAccountData *accountData, AccountsData.values())
		accountData->storeConfiguration();

	foreach (ContactModuleData *moduleData, ModulesData.values())
		moduleData->storeConfiguration();
}

void ContactData::addAccountData(ContactAccountData *accountData)
{
	if (accountData)
		AccountsData.insert(accountData->account(), accountData);
}

ContactAccountData * ContactData::accountData(Account *account)
{
	if (!AccountsData.contains(account))
		return 0;

	return AccountsData[account];
}

bool ContactData::hasStoredAccountData(Account *account)
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return false;

	return !sp->storage()->getUuidNode(sp->point(), "ContactAccountData", account->uuid().toString(), XmlConfigFile::ModeFind).isNull();
}

ContactModuleData * ContactData::moduleData(const QString &key)
{
	return 0;
}

QString ContactData::id(Account *account)
{
	if (AccountsData.contains(account))
		return AccountsData[account]->id();

	return QString::null;
}

Account * ContactData::prefferedAccount()
{
	return AccountsData.count() > 0
		? AccountsData.keys()[0]
		: 0;
}

QList<Account *> ContactData::accounts()
{
	return AccountsData.count() > 0
		? AccountsData.keys()
		: QList<Account *>();
;
}

// properties

bool ContactData::isIgnored()
{
	return Ignored;
}

bool ContactData::setIgnored(bool ignored)
{
	Ignored = ignored;
}

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
