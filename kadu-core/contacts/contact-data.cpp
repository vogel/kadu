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
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"

#include "contact-data.h"

ContactData::ContactData(ContactType type, QUuid uuid) :
		UuidStorableObject("Contact", ContactManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Type(type), Ignored(false), Blocked(false), OfflineTo(false),
		BlockUpdatedSignalCount(0), Updated(false)
{
}

ContactData::~ContactData()
{
}

ContactData * ContactData::loadFromStorage(StoragePoint *contactStoragePoint)
{
	ContactData *result = new ContactData(TypeNormal, QUuid());
	result->setStorage(contactStoragePoint);
	result->load();

	return result;
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

	Type = TypeNormal;
	
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

void ContactData::load()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	StorableObject::load();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	Uuid = QUuid(parent.attribute("uuid"));
	if (parent.hasAttribute("type"))
		Type = (ContactType)parent.attribute("type").toInt();
	else
		Type = TypeNormal;

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
	QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeFind);
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
	Property(Website)

	Ignored = QVariant(configurationStorage->getTextNode(parent, "Ignored")).toBool();
}

#undef Property
#define Property(name) \
	configurationStorage->createTextNode(parent, #name, name);

void ContactData::store()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();
	parent.setAttribute("type", (int)Type);

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
	Property(Website)

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeCreate);
		foreach (Group *group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group->uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ContactGroups");

	configurationStorage->createTextNode(parent, "Ignored", QVariant(Ignored).toString());

	foreach (ContactAccountData *cad, AccountsData.values())
		cad->store();
	storeModuleData();
}

void ContactData::addAccountData(ContactAccountData *accountData)
{
	if (!accountData)
		return;

	emit accountDataAboutToBeAdded(accountData->account());
	AccountsData.insert(accountData->account(), accountData);
	emit accountDataAdded(accountData->account());
}

void ContactData::removeAccountData(Account *account)
{
	emit accountDataAboutToBeRemoved(account);
	AccountsData.remove(account);
	emit accountDataRemoved(account);
}

ContactAccountData * ContactData::accountData(Account *account)
{
	if (!AccountsData.contains(account))
		return 0;

	return AccountsData[account];
}

QList<ContactAccountData *> ContactData::accountDatas()
{
	return AccountsData.values();
}

StoragePoint * ContactData::storagePointForAccountData(Account *account)
{
	StoragePoint *sp = storage();
	if (!sp || !sp->storage())
		return 0;

	QString stringUuid = account->uuid().toString();

	QDomNodeList nodes = sp->storage()->getNodes(sp->point(), "ContactAccountData");
	int count = nodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement element = nodes.at(i).toElement();
		if (element.isNull())
			continue;

		QString accountUuid = sp->storage()->getTextNode(element, "Account");
		if (accountUuid.isEmpty())
			accountUuid = element.attribute("uuid");
		if (accountUuid == stringUuid)
			return new StoragePoint(sp->storage(), element);
	}

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

void ContactData::blockUpdatedSignal()
{
	if (0 == BlockUpdatedSignalCount)
		Updated = false;
	BlockUpdatedSignalCount++;
}

void ContactData::unblockUpdatedSignal()
{
	BlockUpdatedSignalCount--;
	if (0 == BlockUpdatedSignalCount)
		emitUpdated();
}

void ContactData::dataUpdated()
{
	Updated = true;
	emitUpdated();
}

void ContactData::emitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emit updated();
		Updated = false;
	}
}

// properties

bool ContactData::isIgnored()
{
	return Ignored;
}

bool ContactData::setIgnored(bool ignored)
{
	Ignored = ignored;
	dataUpdated();
	return Ignored; // XXX: nie wiem co to
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

bool ContactData::setOfflineTo(Account *account, bool offlineTo)
{
	ContactAccountData *cad = accountData(account);
	if (cad)
		cad->setOfflineTo(offlineTo);
	else
		OfflineTo = offlineTo;

	dataUpdated();

	return true; // XXX
}

bool ContactData::isInGroup(Group *group)
{
	return Groups.contains(group);
}

bool ContactData::showInAllGroup()
{
	foreach (const Group *group, Groups)
		if (0 != group && !group->showInAllGroup())
			return false;
	return true;
}

void ContactData::addToGroup(Group *group)
{
	Groups.append(group);
	dataUpdated();
}

void ContactData::removeFromGroup(Group *group)
{
	Groups.removeAll(group);
	dataUpdated();
}

void ContactData::accountContactDataIdChanged(const QString &id)
{
	ContactAccountData *cad = dynamic_cast<ContactAccountData *>(sender());
	if (cad && cad->account())
		emit accountDataIdChanged(cad->account(), id);
}
