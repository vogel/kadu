/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-manager.h"

#include "contact-shared.h"

ContactShared * ContactShared::loadFromStorage(StoragePoint *storagePoint)
{
	ContactShared *result = new ContactShared(TypeNormal);
	result->setStorage(storagePoint);
	result->load();

	return result;
}

ContactShared::ContactShared(ContactType type, QUuid uuid) :
		UuidStorableObject("Account", ContactManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Type(type),
		BlockUpdatedSignalCount(0), Updated(false)
{
}

ContactShared::~ContactShared()
{
}

void ContactShared::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();

	Uuid = QUuid(loadAttribute<QString>("uuid"));
}

void ContactShared::store()
{
	if (!isValidStorage())
		return;

	storeValue("uuid", Uuid.toString(), true);
}

void ContactShared::dataUpdated()
{
	Updated = true;
	emitUpdated();
}

void ContactShared::emitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emit updated();
		Updated = false;
	}
}
