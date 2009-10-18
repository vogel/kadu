/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "core/core.h"
#include "debug.h"

#include "contact-account-data-manager.h"

ContactAccountDataManager * ContactAccountDataManager::Instance = 0;

ContactAccountDataManager * ContactAccountDataManager::instance()
{
	if (0 == Instance)
		Instance = new ContactAccountDataManager();

	return Instance;
}

ContactAccountDataManager::ContactAccountDataManager()
{
	Core::instance()->configuration()->registerStorableObject(this);
}

ContactAccountDataManager::~ContactAccountDataManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);
}

StoragePoint * ContactAccountDataManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("ContactAccountDatas"));
}

void ContactAccountDataManager::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	// TODO: implement
}

void ContactAccountDataManager::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	foreach (ContactAccountData *cad, ContactAccountDataList)
		cad->store();
}

void ContactAccountDataManager::addContactAccountData(ContactAccountData *cad)
{
	if (!cad)
		return;

	ensureLoaded();

	if (ContactAccountDataList.contains(cad))
		return;

	emit contactAccountDataAboutToBeAdded(cad);
	ContactAccountDataList.append(cad);
	emit contactAccountDataAdded(cad);
}

void ContactAccountDataManager::removeContactAccountData(ContactAccountData *cad)
{
	kdebugf();

	if (!cad)
		return;

	ensureLoaded();

	if (!ContactAccountDataList.contains(cad))
		return;

	emit contactAccountDataAboutToBeRemoved(cad);
	ContactAccountDataList.removeAll(cad);
	emit contactAccountDataRemoved(cad);
}

ContactAccountData * ContactAccountDataManager::byIndex(unsigned int index)
{
	if (index < 0 || index >= count())
		return 0;

	ensureLoaded();

	return ContactAccountDataList.at(index);
}

ContactAccountData * ContactAccountDataManager::byUuid(const QString &uuid)
{
	if (uuid.isEmpty())
		return 0;

	ensureLoaded();

	foreach (ContactAccountData *cad, ContactAccountDataList)
		if (uuid == cad->uuid().toString())
			return cad;

	return 0;
}
