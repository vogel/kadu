/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "debug.h"

#include "contact-manager.h"
#include <protocols/protocol.h>
#include <protocols/protocol-factory.h>

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ContactManager();
		Instance->init();
	}

	return Instance;
}

ContactManager::ContactManager()
{
	Core::instance()->configuration()->registerStorableObject(this);
}

ContactManager::~ContactManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);

	triggerAllAccountsUnregistered();
}

void ContactManager::init()
{
	triggerAllAccountsRegistered();
}

StoragePoint * ContactManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("ContactAccountDatas"));
}

void ContactManager::load(Account account)
{
	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement contactAccountDatasNode = storage()->point();

	if (contactAccountDatasNode.isNull())
		return;

	// TODO 0.6.6: by tag does not work, this works only if childNodes are "Chat"
	QDomNodeList contactAccountDatasNodes = contactAccountDatasNode.childNodes();

	int count = contactAccountDatasNodes.count();

	QString uuid = account.uuid().toString();
	for (int i = 0; i < count; i++)
	{
		QDomElement contactAccountDataElement = contactAccountDatasNodes.at(i).toElement();
		if (contactAccountDataElement.isNull())
			continue;

		if (configurationStorage->getTextNode(contactAccountDataElement, "Account") != uuid)
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, contactAccountDataElement);
		if (!account.protocolHandler())
			return;

		Contact *cad = account.protocolHandler()->protocolFactory()->loadContactAccountData(contactStoragePoint);

		if (cad)
		{
			addContact(cad);
			cad->ensureLoaded();
		}
	}
}

void ContactManager::accountRegistered(Account account)
{
	if (LoadedAccounts.contains(account))
		return;

	load(account);
	LoadedAccounts.append(account);
}

void ContactManager::accountUnregistered(Account account)
{
// 	store(account);
	LoadedAccounts.removeAll(account);
}

void ContactManager::ensureLoaded(Account account)
{
	accountRegistered(account);
}

void ContactManager::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	// TODO: implement
}

void ContactManager::store()
{
	if (!isValidStorage())
		return;

	StorableObject::ensureLoaded();

	foreach (Contact *cad, ContactList)
		cad->store();
}

void ContactManager::addContact(Contact *cad)
{
	if (!cad)
		return;

	StorableObject::ensureLoaded();

	if (ContactList.contains(cad))
		return;

	emit contactAboutToBeAdded(cad);
	ContactList.append(cad);
	emit contactAdded(cad);
}

void ContactManager::removeContact(Contact *cad)
{
	kdebugf();

	if (!cad)
		return;

	StorableObject::ensureLoaded();

	if (!ContactList.contains(cad))
		return;

	emit contactAboutToBeRemoved(cad);
	ContactList.removeAll(cad);
	emit contactRemoved(cad);
}

Contact * ContactManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return 0;

	StorableObject::ensureLoaded();

	return ContactList.at(index);
}

Contact * ContactManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return 0;

	StorableObject::ensureLoaded();

	foreach (Contact *cad, ContactList)
		if (uuid == cad->uuid().toString())
			return cad;

	return 0;
}
