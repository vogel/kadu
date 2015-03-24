/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/group.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "misc/change-notifier.h"
#include "roster/roster-entry.h"
#include "roster/roster-entry-state.h"
#include "storage/storage-point.h"

#include "buddy-shared.h"

BuddyShared * BuddyShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &buddyStoragePoint)
{
	BuddyShared *result = loadFromStorage(buddyStoragePoint);
	result->loadStub();
	return result;
}

BuddyShared * BuddyShared::loadFromStorage(const std::shared_ptr<StoragePoint> &buddyStoragePoint)
{
	BuddyShared *result = new BuddyShared();
	result->setStorage(buddyStoragePoint);
	return result;
}

BuddyShared::BuddyShared(const QUuid &uuid) :
		Shared(uuid), CollectingGarbage(false),
		BirthYear(0), Gender(GenderUnknown), PreferHigherStatuses(true),
		Anonymous(true), Temporary(false), Blocked(false), OfflineTo(false)
{
	BuddyAvatar = new Avatar();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

BuddyShared::~BuddyShared()
{
	ref.ref();
	delete BuddyAvatar;
	BuddyAvatar = 0;
}

void BuddyShared::collectGarbage()
{
	if (CollectingGarbage)
		return;

	CollectingGarbage = true;

	// 1 is for current Buddy
	const int numberOfReferences = 1 + Contacts.length();
	if (numberOfReferences != ref.load())
	{
		CollectingGarbage = false;
		return;
	}

	for (auto &&contact : Contacts)
	{
		Q_ASSERT(!contact.isNull());

		// 1 is for current BuddyShared
		const int contactNumberOfReferences = 1;
		if (contactNumberOfReferences != contact.data()->ref.load())
		{
			CollectingGarbage = false;
			return;
		}
	}

	for (auto &&contact : Contacts)
		contact.setOwnerBuddy(Buddy::null);

	CollectingGarbage = false;
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
	set##name(CustomData.value(#old_name)); \
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
	for (auto &&group : groups)
		doAddToGroup(GroupManager::instance()->byName(group));

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
	if (!isValidStorage())
		return;

	Shared::load();

	ConfigurationApi *configurationStorage = storage()->storage();
	QDomElement parent = storage()->point();

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues", ConfigurationApi::ModeFind);
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
	QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", ConfigurationApi::ModeFind);
	if (!groupsNode.isNull())
	{
		QDomNodeList groupsList = groupsNode.elementsByTagName("Group");

		count = groupsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement groupElement = groupsList.at(i).toElement();
			if (groupElement.isNull())
				continue;
			doAddToGroup(GroupManager::instance()->byUuid(groupElement.text()));
		}
	}

	setBuddyAvatar(AvatarManager::instance()->byUuid(loadValue<QString>("Avatar")));
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

	// Some crazy bug causes entries like <Buddy uuid="xxx..."/> to be stored to the configuration file
	// after using open-chat-with. We must not treat them as not anonymous (i.e., present on contact list) buddies,
	// hence this workaround.
	Anonymous = Display.isEmpty();
}

void BuddyShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	ConfigurationApi *configurationStorage = storage()->storage();
	QDomElement parent = storage()->point();

	QDomElement customDataValues = configurationStorage->getNode(parent, "CustomDataValues", ConfigurationApi::ModeCreate);

	for (QMap<QString, QString>::const_iterator it = CustomData.constBegin(), end = CustomData.constEnd(); it != end; ++it)
		configurationStorage->createNamedTextNode(customDataValues, "CustomDataValue", it.key(), it.value());

	if (!BuddyAvatar->uuid().isNull())
		storeValue("Avatar", BuddyAvatar->uuid().toString());
	else
		removeValue("Avatar");

	// should not happen, but who knows...
	if (Display.isEmpty())
		Display = uuid().toString();
	storeValue("Display", Display);

	storeValue("FirstName", FirstName);
	storeValue("LastName", LastName);
	storeValue("NickName", NickName);
	storeValue("HomePhone", HomePhone);
	storeValue("Mobile", Mobile);
	storeValue("Email", Email);
	storeValue("Website", Website);
	storeValue("Blocked", Blocked);
	storeValue("OfflineTo", OfflineTo);
	storeValue("Gender", (int)Gender);
	storeValue("PreferHigherStatuses", PreferHigherStatuses);

	// This buddy can't be anonymous, otherwise we wouldn't be storing them. Though,
	// we need to store Anonymous=false, otherwise we will break downgrade to Kadu <0.11.0.
	// TODO when we change configuration format (or just file name): remove it
	storeValue("Anonymous", false);

	if (!Groups.isEmpty())
	{
		QDomElement groupsNode = configurationStorage->getNode(parent, "ContactGroups", ConfigurationApi::ModeCreate);
		for (auto &&group : Groups)
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
	setAnonymous(true);

	Contacts.clear();
	Groups.clear();

	AvatarManager::instance()->removeItem(*BuddyAvatar);
	setBuddyAvatar(Avatar::null);
}

int BuddyShared::priorityForNewContact()
{
	// anonymous (default) buddies should have only contacts without priority
	if (isAnonymous())
		return -1;

	return Contacts.isEmpty()
	        ? 0
	        : Contacts.at(Contacts.count() - 1).priority() + 1;
}

void BuddyShared::addContact(const Contact &contact)
{
	ensureLoaded();

	if (!contact || Contacts.contains(contact))
		return;

	if (-1 == contact.priority())
		contact.setPriority(priorityForNewContact());

	emit contactAboutToBeAdded(contact);

	Contacts.append(contact);
	sortContacts();

	emit contactAdded(contact);

	connect(contact, SIGNAL(priorityUpdated()), &changeNotifier(), SLOT(notify()));
	changeNotifier().notify();
}

void BuddyShared::removeContact(const Contact &contact)
{
	ensureLoaded();

	if (!contact || !Contacts.contains(contact))
		return;

	disconnect(contact, SIGNAL(priorityUpdated()), &changeNotifier(), SLOT(notify()));

	emit contactAboutToBeRemoved(contact);
	Contacts.removeAll(contact);
	emit contactRemoved(contact);

	normalizePriorities();

	changeNotifier().notify();
}

QVector<Contact> BuddyShared::contacts(const Account &account)
{
	ensureLoaded();

	QVector<Contact> contacts;
	for (auto &&contact :  Contacts)
		if (contact.contactAccount() == account)
			contacts.append(contact);

	return contacts;
}

const QList<Contact> & BuddyShared::contacts()
{
	ensureLoaded();

	return Contacts;
}

QString BuddyShared::id(const Account &account)
{
	ensureLoaded();

	QVector<Contact> contactslist;
	contactslist = contacts(account);
	if (!contactslist.isEmpty())
		return contactslist.at(0).id();

	return QString();
}

static bool contactPriorityLessThan(const Contact &c1, const Contact &c2)
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
	for (auto &&contact : Contacts)
		contact.setPriority(priority++);
}

void BuddyShared::avatarUpdated()
{
	changeNotifier().notify();
}

void BuddyShared::setBuddyAvatar(const Avatar &buddyAvatar)
{
	if (*BuddyAvatar == buddyAvatar)
		return;

	if (*BuddyAvatar)
		disconnect(*BuddyAvatar, 0, this, 0);

	*BuddyAvatar = buddyAvatar;
	changeNotifier().notify();

	if (*BuddyAvatar)
		connect(*BuddyAvatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
}

void BuddyShared::setDisplay(const QString &display)
{
	ensureLoaded();

	if (Display != display)
	{
		Display = display;
		changeNotifier().notify();
		markContactsDirty();

		emit displayUpdated();
	}
}

void BuddyShared::setGroups(const QSet<Group> &groups)
{
	ensureLoaded();

	if (Groups == groups)
		return;

	QSet<Group> groupsToRemove = Groups;

	for (auto &&group : groups)
		if (!groupsToRemove.remove(group))
			doAddToGroup(group);

	for (auto &&group : groupsToRemove)
		doRemoveFromGroup(group);

	changeNotifier().notify();
	markContactsDirty();
}

bool BuddyShared::isInGroup(const Group &group)
{
	ensureLoaded();

	return Groups.contains(group);
}

bool BuddyShared::showInAllGroup()
{
	ensureLoaded();

	for (auto &&group : Groups)
		if (group && !group.showInAllGroup())
			return false;

	return true;
}

bool BuddyShared::doAddToGroup(const Group &group)
{
	if (!group || Groups.contains(group))
		return false;

	Groups.insert(group);
	connect(group, SIGNAL(nameChanged()), this, SLOT(markContactsDirty()));
	connect(group, SIGNAL(groupAboutToBeRemoved()), this, SLOT(groupAboutToBeRemoved()));

	return true;
}

bool BuddyShared::doRemoveFromGroup(const Group &group)
{
	if (!Groups.remove(group))
		return false;

	disconnect(group, 0, this, 0);

	return true;
}

void BuddyShared::addToGroup(const Group &group)
{
	ensureLoaded();

	if (doAddToGroup(group))
	{
		changeNotifier().notify();
		markContactsDirty();
	}
}

void BuddyShared::removeFromGroup(const Group &group)
{
	ensureLoaded();

	if (doRemoveFromGroup(group))
	{
		changeNotifier().notify();
		markContactsDirty();
	}
}

void BuddyShared::groupAboutToBeRemoved()
{
	Group group(sender());
	if (!group.isNull())
		removeFromGroup(group);
}

bool BuddyShared::isEmpty(bool checkOnlyForContacts)
{
	ensureLoaded();

	if (checkOnlyForContacts)
		return Contacts.isEmpty();
	else
		return Contacts.isEmpty() && HomePhone.isEmpty() && Mobile.isEmpty() && Website.isEmpty() && Email.isEmpty();
}

void BuddyShared::markContactsDirty()
{
	ensureLoaded();

	for (auto &&contact : Contacts)
		if (contact.rosterEntry())
			contact.rosterEntry()->setHasLocalChanges();
}

quint16 BuddyShared::unreadMessagesCount()
{
	ensureLoaded();

	quint16 result = 0;
	for (auto &&contact : Contacts)
		result += contact.unreadMessagesCount();

	return result;
}

std::shared_ptr<StoragePoint> BuddyShared::createStoragePoint()
{
	// TODO: fix this, it is only a workaround for an empty buddy on list
	if (Core::instance()->myself() == Buddy(this))
		return {};
	else
		return Shared::createStoragePoint();
}

KaduShared_PropertyPtrReadDef(BuddyShared, Avatar, buddyAvatar, BuddyAvatar)

#include "moc_buddy-shared.cpp"
