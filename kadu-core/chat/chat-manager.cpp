/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat.h"
#include "chat-manager.h"
#include "simple-chat.h"

ChatManager * ChatManager::Instance = 0;

ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
		Instance = new ChatManager();

	return Instance;
}

StoragePoint * ChatManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Chats"));
}

void ChatManager::loadConfigurationForAccount(Account *account)
{
	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement transfersNewNode = storage()->point();

	if (transfersNewNode.isNull())
		return;

	QDomNodeList chatNodes = transfersNewNode.elementsByTagName("Chat");

	int count = chatNodes.count();

	QString uuid = account->uuid().toString();
	for (int i = 0; i < count; i++)
	{
		QDomElement chatElement = chatNodes.at(i).toElement();
		if (chatElement.isNull())
			continue;

		if (configurationStorage->getTextNode(chatElement, "Account") != uuid)
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, chatElement);
		Chat *chat = Chat::loadFromStorage(contactStoragePoint);

		if (chat)
			addChat(chat);
// 		else TODO: remove?
// 			transfersNewNode.removeChild(chatElement);
	}
}

void ChatManager::storeConfigurationForAccount(Account *account)
{
	foreach (QList<Chat *> list, Chats.values())
		foreach (Chat *chat, list) 
			if (chat->account() == account)
				chat->storeConfiguration();
}

void ChatManager::addChat(Chat *chat)
{
	emit chatAboutToBeAdded(chat);
	if (!Chats.contains(chat->account()))
		Chats[chat->account()] = QList<Chat *>();
	Chats[chat->account()].append(chat);
	emit chatAdded(chat);
}

void ChatManager::removeChat(Chat *chat)
{
	emit chatAboutToBeRemoved(chat);
	if (!Chats.contains(chat->account()))
		return;
	Chats[chat->account()].removeOne(chat);
	chat->removeFromStorage();
	emit chatRemoved(chat);

	delete chat;
}

QList<Chat *> ChatManager::chatsForAccount(Account *account)
{
	if (!Chats.contains(account))
		return QList<Chat *>();
	return Chats[account]; 
}

void ChatManager::accountRegistered(Account *account)
{
	loadConfigurationForAccount(account);
}

void ChatManager::accountUnregistered(Account *account)
{
	storeConfigurationForAccount(account);
}
