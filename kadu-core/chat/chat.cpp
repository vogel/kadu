/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "protocols/protocol.h"

#include "chat.h"
#include "chat-manager.h"

Chat::Chat(Account *currentAccount, QUuid uuid)
	: CurrentAccount(currentAccount), Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Chat::~Chat()
{
}

QUuid Chat::uuid() const
{
	return QUuid();
}

StoragePoint * Chat::createStoragePoint() const
{
	StoragePoint *parent = ChatManager::instance()->storage();
	if (!parent)
		return 0;

	QDomElement chatNode = parent->storage()->getUuidNode(parent->point(), "Chat", Uuid.toString());
	return new StoragePoint(parent->storage(), chatNode);
}

Chat * Chat::loadFromStorage(StoragePoint *chatStoragePoint)
{
	if (!chatStoragePoint || !chatStoragePoint->storage())
		return 0;

	XmlConfigFile *storage = chatStoragePoint->storage();
	QDomElement point = chatStoragePoint->point();

	Account *account = AccountManager::instance()->byUuid(QUuid(storage->getTextNode(point, "Account")));
	if (!account)
		return 0;

	return account->protocol()->loadChatFromStorage(chatStoragePoint);
}

void Chat::loadConfiguration()
{
	if (!isValidStorage())
		return;
	CurrentAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));
}

void Chat::storeConfiguration()
{
	if (!isValidStorage())
		return;
	storeValue("Account", CurrentAccount->uuid().toString());
}
