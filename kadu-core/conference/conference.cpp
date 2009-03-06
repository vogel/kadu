/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "accounts/account_manager.h"
#include "protocols/protocol.h"

#include "conference.h"
#include "conference-manager.h"

Conference::Conference(Account *currentAccount, QUuid uuid)
	: CurrentAccount(currentAccount), Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Conference::~Conference()
{
}

QUuid Conference::uuid() const
{
	return QUuid();
}

StoragePoint * Conference::createStoragePoint() const
{
	StoragePoint *parent = ConferenceManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement conferenceNode = parent->storage()->getUuidNode(parent->point(), "Conference", Uuid.toString());
	return new StoragePoint(parent->storage(), conferenceNode);
}

Conference * Conference::loadFromStorage(StoragePoint *conferenceStoragePoint)
{
	if (!conferenceStoragePoint || !conferenceStoragePoint->storage())
		return 0;

	XmlConfigFile *storage = conferenceStoragePoint->storage();
	QDomElement point = conferenceStoragePoint->point();

	Account *account = AccountManager::instance()->byUuid(QUuid(storage->getTextNode(point, "Account")));
	if (!account)
		return 0;

	return account->protocol()->loadConferenceFromStorage(conferenceStoragePoint);
}

void Conference::loadConfiguration()
{
	if (!isValidStorage())
		return;
	CurrentAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));
}

void Conference::storeConfiguration()
{
	if (!isValidStorage())
		return;
	storeValue("Account", CurrentAccount->uuid().toString());
}
