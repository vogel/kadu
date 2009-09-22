/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "chat.h"
#include "chat-manager.h"
#include "simple-chat.h"

ChatManager * ChatManager::Instance = 0;

ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
	{
		Instance = new ChatManager();
		Instance->init();
	}

	return Instance;
}

ChatManager::ChatManager() :
		StorableObject(true)
{
	Core::instance()->configuration()->registerStorableObject(this);
}

ChatManager::~ChatManager()
{
	Core::instance()->configuration()->unregisterStorableObject(this);

	triggerAllAccountsUnregistered();
}

void ChatManager::init()
{
	triggerAllAccountsRegistered();
}

StoragePoint * ChatManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Chats"));
}

void ChatManager::load(Account *account)
{
	if (!isValidStorage())
		return;

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement chatsNode = storage()->point();

	if (chatsNode.isNull())
		return;

	// TODO 0.6.6: by tag does not work, this works only if childNodes are "Chat"
	QDomNodeList chatNodes = chatsNode.childNodes();
	//QDomNodeList chatNodes = chatsNode.elementsByTagName("Chat");

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
	}
}

void ChatManager::store(Account *account)
{
	foreach (Chat *chat, Chats[account])
		chat->store();
}

void ChatManager::store()
{
	foreach (QList<Chat *> list, Chats.values())
		foreach (Chat *chat, list)
			chat->store();
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
	if (!Chats.contains(chat->account()))
		return;

	emit chatAboutToBeRemoved(chat);
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

Chat * ChatManager::byUuid(QUuid uuid)
{
	foreach (QList<Chat *> list, Chats.values())
		foreach (Chat *chat, list)
			if (chat->uuid() == uuid)
				return chat;
	return 0;
}

void ChatManager::accountRegistered(Account *account)
{
	load(account);
}

void ChatManager::accountUnregistered(Account *account)
{
	store(account);
}
