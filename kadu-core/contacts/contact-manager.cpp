/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"

#include "configuration/configuration-manager.h"
#include "configuration/storage-point.h"
#include "contacts/group-manager.h"
#include "core/core.h"

#include "contact.h"
#include "contact-list.h"
#include "debug.h"
#include "group.h"
#include "xml_config_file.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager *  ContactManager::instance()
{
	if (0 == Instance)
		Instance = new ContactManager();

	return Instance;
}

ContactManager::ContactManager()
{
	Core::instance()->configuration()->registerStorableObject(this);

	connect(GroupManager::instance(), SIGNAL(groupAboutToBeRemoved(Group *)),
			this, SLOT(groupRemoved(Group *)));
}

ContactManager::~ContactManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);
}

StoragePoint * ContactManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("ContactsNew"));
}

void ContactManager::importConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement contactsNode = configurationStorage->getNode("Contacts", XmlConfigFile::ModeFind);
	if (contactsNode.isNull())
		return;

	QDomNodeList contactsNodes = configurationStorage->getNodes(contactsNode, "Contact");
	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactElement = contactsNodes.item(i).toElement();
		if (contactElement.isNull())
			continue;

		Contact contact;
		contact.importConfiguration(configurationStorage, contactElement);

		addContact(contact);
	}
}

void ContactManager::load()
{
	StorableObject::load();

	if (xml_config_file->getNode("ContactsNew", XmlConfigFile::ModeFind).isNull())
	{
		importConfiguration(xml_config_file);
		return;
	}

	if (!isValidStorage())
		return;

	QDomElement contactsNewNode = storage()->point();
	QDomNodeList contactsNodes = contactsNewNode.elementsByTagName("Contact");

	int count = contactsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomNode contactNode = contactsNodes.at(i);
		QDomElement contactElement = contactNode.toElement();
		if (contactElement.isNull())
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(storage()->storage(), contactElement);
		addContact(Contact::loadFromStorage(contactStoragePoint));
	}
}

void ContactManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	foreach (Contact contact, Contacts)
		if (!contact.isNull() && !contact.isAnonymous())
			contact.store();
}

void ContactManager::addContact(Contact contact)
{
	if (contact.isNull())
		return;

	ensureLoaded();

	emit contactAboutToBeAdded(contact);
	Contacts.append(contact);
	connect(contact.data(), SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	emit contactAdded(contact);
}

void ContactManager::removeContact(Contact contact)
{
	kdebugf();
	if (contact.isNull())
		return;

	ensureLoaded();

	emit contactAboutToBeRemoved(contact);
	disconnect(contact.data(), SIGNAL(updated()), this, SLOT(contactDataUpdated()));
	Contacts.removeAll(contact);
	contact.removeFromStorage();
	emit contactRemoved(contact);
	contact.setType(Contact::TypeAnonymous);

	kdebugf();
}

Contact ContactManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return Contact::null;

	ensureLoaded();

	return Contacts.at(index);
}

Contact ContactManager::byId(Account *account, const QString &id)
{
	if (id.isEmpty() || 0 == account)
		return Contact::null;

	ensureLoaded();

	foreach (Contact contact, Contacts)
	{
		if (id == contact.id(account))
			return contact;
	}

	Contact anonymous = account->createAnonymous(id);
	addContact(anonymous);

	return anonymous;
}

Contact ContactManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return Contact::null;

	ensureLoaded();

	foreach (Contact contact, Contacts)
		if (uuid == contact.uuid().toString())
			return contact;

	return Contact::null;
}

Contact ContactManager::byDisplay(const QString &display)
{
	if (display.isEmpty())
		return Contact::null;

	ensureLoaded();

	foreach (Contact contact, Contacts)
	{
		if (display == contact.display())
			return contact;
	}

	return Contact::null;
}

void ContactManager::blockUpdatedSignal(Contact &contact)
{
	contact.data()->blockUpdatedSignal();
}

void ContactManager::unblockUpdatedSignal(Contact &contact)
{
	contact.data()->unblockUpdatedSignal();
}

ContactList ContactManager::contacts()
{
	ensureLoaded();
	return Contacts;
}

ContactList ContactManager::contacts(Account *account, bool includeAnonymous)
{
	ContactList result;

	foreach (Contact contact, Contacts)
		if (contact.accountData(account) && (includeAnonymous || !contact.isAnonymous()))
			result << contact;

	ensureLoaded();

	return result;
}

void ContactManager::contactDataUpdated()
{
	ContactData *cd = dynamic_cast<ContactData *>(sender());
	if (!cd)
		return;

	foreach (Contact contact, Contacts)
		if (cd == contact.data())
		{
			emit contactUpdated(contact);
			return;
		}
}

void ContactManager::groupRemoved(Group *group)
{
	foreach (Contact contact, Contacts)
		contact.removeFromGroup(group);
}
