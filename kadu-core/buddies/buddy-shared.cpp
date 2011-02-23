/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QVariant>
#include <QtXml/QDomNamedNodeMap>

#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "contacts/contact-manager.h"
#include "storage/storage-point.h"

#include "buddy-shared.h"

BuddyShared * BuddyShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint)
{
	BuddyShared *result = loadFromStorage(buddyStoragePoint);
	result->loadStub();
	return result;
}

BuddyShared * BuddyShared::loadFromStorage(const QSharedPointer<StoragePoint> &buddyStoragePoint)
{
	BuddyShared *result = new BuddyShared();
	result->setStorage(buddyStoragePoint);
	return result;
}

BuddyShared::BuddyShared(QUuid uuid) :
		Shared(uuid),
		BirthYear(0), Gender(GenderUnknown),
		Anonymous(true), Blocked(false), OfflineTo(false)
{
}

BuddyShared::~BuddyShared()
{
	ref.ref();
}

StorableObject * BuddyShared::storageParent()
{
	return BuddyManager::instance();
}

QString BuddyShared::storageNodeName()
{
	return QLatin1String("Buddy");
}

#define ImportProperty(name, old_name) \
	set##name(CustomData[#old_name]); \
	CustomData.remove(#old_name);

void BuddyShared::importConfiguration(const QDomElement &parent)
{
	QDomNamedNodeMap attributes = parent.attributes();
	int count = attributes.count();

	for (int i = 0; i < count; i++)
	{
		QDomAttr attribute = attributes.item(i).toAttr();
		CustomData.insert(attribute.name(), attribute.value());
	}

	Anonymous = false;

	importConfiguration();
}

void BuddyShared::importConfiguration()
{
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
	QSharedPointer<StoragePoint> sp(storage());
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
		Anonymous = loadValue<bool>("Anonymous", true);

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

	BuddyAvatar = AvatarManager::instance()->byUuid(loadValue<QString>("Avatar"));
	Display = loadValue<QString>("Display");
	FirstName = loadValue<QString>("FirstName");
	LastName = loadValue<QString>("LastName");
	NickName = loadValue<QString>("NickName");
	HomePhone = loadValue<QString>("HomePhone");
	Mobile = loadValue<QString>("Mobile");
	Email = loadValue<QString>("Email");
	Website = loadValue<QString>("Website");
	Blocked = loadValue<bool>("Blocked", false);
	OfflineTo = loadValue<bool>("OfflineTo", false);
	Gender = (BuddyGender)loadValue<int>("Gender", 0);
	PreferHigherStatuses = loadValue<bool>("PreferHigherStatuses", true);
}

void BuddyShared::store()
{
	ensureLoaded();

	QSharedPointer<StoragePoint> sp(storage());
	if (!sp)
		return;

	Shared::store();

	XmlConfigFile *configurationStorage = sp->storage();
	QDomElement parent = sp->point();

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues", XmlConfigFile::ModeCreate);

	foreach (const QString &key, CustomData.keys())
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", key, CustomData[key]);

	if (BuddyAvatar.uuid().isNull())
		removeValue("Avatar");
	else
		storeValue("Avatar", BuddyAvatar.uuid().toString());

	storeValue("Display", Display);
	storeValue("FirstName", FirstName);
	storeValue("LastName", LastName);
	storeValue("NickName", NickName);
	storeValue("HomePhone", HomePhone);
	storeValue("Mobile", Mobile);
	storeValue("Email", Email);
	storeValue("Website", Website);
	storeValue("Anonymous", Anonymous);
	storeValue("Blocked", Blocked);
	storeValue("OfflineTo", OfflineTo);
	storeValue("Gender", (int)Gender);
	storeValue("PreferHigherStatuses", PreferHigherStatuses);

	if (Groups.count())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", XmlConfigFile::ModeCreate);
		foreach (const Group &group, Groups)
			configurationStorage->appendTextNode(groupsNode, "Group", group.uuid().toString());
	}
	else
		configurationStorage->removeNode(parent, "ContactGroups");
}

bool BuddyShared::shouldStore()
{
	ensureLoaded();

    return UuidStorableObject::shouldStore() && !isAnonymous();
}

void BuddyShared::aboutToBeRemoved()
{
	/* NOTE: This guard is needed to delay deleting this object when removing
	 * Buddy from Contact which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen now because we have still
	 * some things to do here.
	 */
	Buddy guard(this);

	setAnonymous(true);

	foreach (Contact contact, Contacts)
		contact.setOwnerBuddy(Buddy::null);

	Contacts.clear();
	Groups.clear();

	AvatarManager::instance()->removeItem(BuddyAvatar);
	BuddyAvatar = Avatar::null;
}

void BuddyShared::addContact(Contact contact)
{
	ensureLoaded();

	if (contact.isNull() || Contacts.contains(contact))
		return;

	emit contactAboutToBeAdded(contact);

	if (contact.priority() == -1)
	{
		int last = Contacts.count() > 1
				? Contacts.at(Contacts.count() - 1).priority()
				: 0;
		contact.setPriority(last);
	}

	Contacts.append(contact);

	sortContacts();

	emit contactAdded(contact);

	dataUpdated();
}

void BuddyShared::removeContact(Contact contact)
{
	ensureLoaded();

	if (contact.isNull() || !Contacts.contains(contact))
		return;

	emit contactAboutToBeRemoved(contact);
	Contacts.removeAll(contact);
	emit contactRemoved(contact);

	normalizePriorities();

	dataUpdated();
}

QList<Contact> BuddyShared::contacts(const Account &account)
{
	ensureLoaded();

	QList<Contact> contacts;

	foreach (const Contact &contact, Contacts)
		if (contact.contactAccount() == account)
			contacts.append(contact);

	return contacts;
}

QList<Contact> BuddyShared::contacts()
{
	ensureLoaded();

	return Contacts;
}

QString BuddyShared::id(const Account &account)
{
	ensureLoaded();

	QList<Contact> contactslist;
	contactslist = contacts(account);
	if (contactslist.count() > 0)
		return contactslist.at(0).id();

	return QString();
}

bool contactPriorityLessThan(const Contact &c1, const Contact &c2)
{
	return c1.priority() < c2.priority();
}

void BuddyShared::sortContacts()
{
	qStableSort(Contacts.begin(), Contacts.end(), contactPriorityLessThan);
}

void BuddyShared::normalizePriorities()
{
	int priority = 0;
	foreach (Contact contact, Contacts)
		contact.setPriority(priority++);
}

void BuddyShared::emitUpdated()
{
	emit updated();
}

// properties

bool BuddyShared::isInGroup(const Group &group)
{
	ensureLoaded();

	return Groups.contains(group);
}

bool BuddyShared::showInAllGroup()
{
	ensureLoaded();

	foreach (const Group &group, Groups)
		if (!group.isNull() && !group.showInAllGroup())
			return false;

	return true;
}

void BuddyShared::addToGroup(const Group &group)
{
	ensureLoaded();

	if (Groups.contains(group) || group.isNull())
		return;

	Groups.append(group);
	dataUpdated();
}

void BuddyShared::removeFromGroup(const Group &group)
{
	ensureLoaded();

	Groups.removeAll(group);
	dataUpdated();
}

bool BuddyShared::isEmpty()
{
	ensureLoaded();

	return Contacts.isEmpty() && HomePhone.isEmpty() && Mobile.isEmpty() && Website.isEmpty() && Email.isEmpty();
}
