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
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"

#include "buddy-shared.h"

BuddyShared * BuddyShared::loadFromStorage(StoragePoint *contactStoragePoint)
{
	BuddyShared *result = new BuddyShared(TypeNormal, QUuid());
	result->setStorage(contactStoragePoint);
	result->load();
	
	return result;
}

BuddyShared::BuddyShared(BuddyType type, QUuid uuid) :
		Shared(uuid, "Buddy", BuddyManager::instance()), Type(type),
		Ignored(false), Blocked(false), OfflineTo(false)
{
}

BuddyShared::~BuddyShared()
{
}

#undef Property
#define Property(name, old_name) \
	set##name(CustomData[#old_name]); \
	CustomData.remove(#old_name);

void BuddyShared::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
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

void BuddyShared::load()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	Shared::load();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	if (parent.hasAttribute("type"))
		Type = (BuddyType)parent.attribute("type").toInt();
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

	Display = loadValue<QString>("Display");
	FirstName = loadValue<QString>("FirstName");
	LastName = loadValue<QString>("LastName");
	NickName = loadValue<QString>("NickName");
	HomePhone = loadValue<QString>("HomePhone");
	Mobile = loadValue<QString>("Mobile");
	Email = loadValue<QString>("Email");
	Website = loadValue<QString>("Website");
	Ignored = loadValue<bool>("Ignored", false);
	Blocked = loadValue<bool>("Blocked", false);
	OfflineTo = loadValue<bool>("OfflineTo", false);
}

void BuddyShared::store()
{
	StoragePoint *sp = storage();
	if (!sp)
		return;

	Shared::store();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();
	parent.setAttribute("type", (int)Type);

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues");

	foreach (const QString &key, CustomData.keys())
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);

	storeValue("Display", Display);
	storeValue("FirstName", FirstName);
	storeValue("LastName", LastName);
	storeValue("NickName", NickName);
	storeValue("HomePhone", HomePhone);
	storeValue("Mobile", Mobile);
	storeValue("Email", Email);
	storeValue("Website", Website);
	storeValue("Ignored", Ignored);
	storeValue("Blocked", Blocked);
	storeValue("OfflineTo", OfflineTo);

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeCreate);
		foreach (Group *group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group->uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ContactGroups");

	configurationStorage->createTextNode(parent, "Ignored", QVariant(Ignored).toString());

	storeModuleData();
}

void BuddyShared::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	emit contactAboutToBeAdded(contact.contactAccount());
	Contacts.insert(contact.contactAccount(), contact);
	ContactManager::instance()->addContact(contact);
	emit contactAdded(contact.contactAccount());
}

void BuddyShared::removeContact(Contact contact)
{
	if (Contacts[contact.contactAccount()] == contact)
		removeContact(contact.contactAccount());
}

void BuddyShared::removeContact(Account account)
{
	emit contactAboutToBeRemoved(account);
	ContactManager::instance()->removeContact(Contacts[account]);
	Contacts.remove(account);
	emit contactRemoved(account);
}

Contact BuddyShared::contact(Account account)
{
	if (!Contacts.contains(account))
		return Contact::null;

	return Contacts[account];
}

QList<Contact> BuddyShared::contacts()
{
	return Contacts.values();
}

QString BuddyShared::id(Account account)
{
	if (Contacts.contains(account))
		return Contacts[account].id();

	return QString::null;
}

Account BuddyShared::prefferedAccount()
{
	return Contacts.count() > 0
		? Contacts.keys()[0]
		: Account::null;
}

QList<Account> BuddyShared::accounts()
{
	return Contacts.count() > 0
			? Contacts.keys()
			: QList<Account>();
}

void BuddyShared::emitUpdated()
{
	emit updated();
}

// properties

bool BuddyShared::isInGroup(Group *group)
{
	return Groups.contains(group);
}

bool BuddyShared::showInAllGroup()
{
	foreach (const Group *group, Groups)
		if (0 != group && !group->showInAllGroup())
			return false;
	return true;
}

void BuddyShared::addToGroup(Group *group)
{
	Groups.append(group);
	dataUpdated();
}

void BuddyShared::removeFromGroup(Group *group)
{
	Groups.removeAll(group);
	dataUpdated();
}

void BuddyShared::accountContactDataIdChanged(const QString &id)
{
	Contact contact = *(dynamic_cast<Contact *>(sender()));
	if (!contact.isNull() && !contact.contactAccount().isNull())
		emit contactIdChanged(contact.contactAccount(), id);
}
