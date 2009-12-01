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
#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "storage/storage-point.h"

#include "buddy-shared.h"

BuddyShared * BuddyShared::loadFromStorage(StoragePoint *contactStoragePoint)
{
	BuddyShared *result = new BuddyShared();
	result->setStorage(contactStoragePoint);
	result->load();
	
	return result;
}

BuddyShared::BuddyShared(QUuid uuid) :
		QObject(BuddyManager::instance()),
		Shared(uuid, "Buddy", BuddyManager::instance()),
		Anonymous(false), Ignored(false), Blocked(false), OfflineTo(false)
{
}

BuddyShared::~BuddyShared()
{
}

#define ImportProperty(name, old_name) \
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

	Anonymous = false;

	QStringList groups = CustomData["groups"].split(',', QString::SkipEmptyParts);
	foreach (const QString &group, groups)
		Groups << GroupManager::instance()->byName(group);

	CustomData.remove("groups");

	ImportProperty(Display, altnick)
	ImportProperty(FirstName, first_name)
	ImportProperty(LastName, last_name)
	ImportProperty(NickName, nick_name)
	ImportProperty(HomePhone, home_phone)
	ImportProperty(Mobile, mobile)
	ImportProperty(Email, email)
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
	{
		Anonymous = (1 == parent.attribute("type").toInt());
		parent.removeAttribute("type");
	}
	else
		Anonymous = loadValue<bool>("Anonymous");

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
			Group group = GroupManager::instance()->byUuid(groupElement.text());
			if (!group.isNull())
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
	storeValue("Anonymous", Anonymous);
	storeValue("Ignored", Ignored);
	storeValue("Blocked", Blocked);
	storeValue("OfflineTo", OfflineTo);

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeCreate);
		foreach (Group group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group.uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ContactGroups");

	configurationStorage->createTextNode(parent, "Ignored", QVariant(Ignored).toString());

	storeModuleData();
}

void BuddyShared::addContact(Contact contact)
{
	if (contact.isNull() || Contacts.contains(contact))
		return;

	emit contactAboutToBeAdded(contact);
	Contacts.append(contact);
	ContactManager::instance()->addItem(contact);
	emit contactAdded(contact);
}

void BuddyShared::removeContact(Contact contact)
{
	if (contact.isNull() || !Contacts.contains(contact))
		return;

	emit contactAboutToBeRemoved(contact);
	ContactManager::instance()->removeItem(contact);
	Contacts.removeAll(contact);
	emit contactRemoved(contact);
}

QList<Contact> BuddyShared::contacts(Account account)
{
	QList<Contact> contacts;

	foreach (const Contact &contact, Contacts)
		if (contact.contactAccount() == account)
			contacts.append(contact);

	// TODO 0.6.6 : if count() > 1 ... sort out! (0 - preffered)
	return contacts;
}

QList<Contact> BuddyShared::contacts()
{
	return Contacts;
}

QString BuddyShared::id(Account account)
{
	QList<Contact> contactslist;
	contactslist = contacts(account);
	if (contactslist.count() > 0)
		return contactslist[0].id();

	return QString::null;
}
Contact BuddyShared::prefferedContact()
{
	// TODO 0.6.6: implement it to have most available contact
	return Contacts.count() > 0
		? Contacts[0]
		: Contact::null;
}

Account BuddyShared::prefferedAccount()
{
	return prefferedContact().contactAccount();
}

QList<Account> BuddyShared::accounts()
{
	QList<Account> accounts;

	foreach (const Contact &contact, Contacts)
		if (!accounts.contains(contact.contactAccount()))
			accounts.append(contact.contactAccount());

	return accounts;
}

void BuddyShared::emitUpdated()
{
	emit updated();
}

// properties

bool BuddyShared::isInGroup(Group group)
{
	return Groups.contains(group);
}

bool BuddyShared::showInAllGroup()
{
	foreach (const Group group, Groups)
		if (!group.isNull() && !group.showInAllGroup())
			return false;
	return true;
}

void BuddyShared::addToGroup(Group group)
{
	Groups.append(group);
	dataUpdated();
}

void BuddyShared::removeFromGroup(Group group)
{
	Groups.removeAll(group);
	dataUpdated();
}

void BuddyShared::accountContactDataIdChanged(const QString &id)
{
	Contact contact = *(dynamic_cast<Contact *>(sender()));
	if (!contact.isNull() && !contact.contactAccount().isNull())
		emit contactIdChanged(contact, id);
}
