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

Chat::Chat(StoragePoint *storage) :
		UuidStorableObject(storage)
{

}

Chat::Chat(Account *currentAccount, QUuid uuid) :
		UuidStorableObject("Chat", ChatManager::instance()), CurrentAccount(currentAccount), Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Chat::~Chat()
{
}

void Chat::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();

	Uuid = loadAttribute<QString>("uuid");
	CurrentAccount = AccountManager::instance()->byUuid(QUuid(loadValue<QString>("Account")));
}

void Chat::store()
{
	if (!isValidStorage())
		return;

	storeValue("Account", CurrentAccount->uuid().toString());
}
