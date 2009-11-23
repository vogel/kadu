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

ChatManager * ChatManager::Instance = 0;

ChatManager *  ChatManager::instance()
{
	if (0 == Instance)
		Instance = new ChatManager();

	return Instance;
}

ChatManager::ChatManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
}

ChatManager::~ChatManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

StoragePoint * ChatManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Chats"));
}

void ChatManager::load()
{
	if (!isValidStorage())
		return;

	if (!needsLoad())
		return;

	StorableObject::load();

	XmlConfigFile *configurationStorage = storage()->storage();
	QDomElement chatsNode = storage()->point();

	if (chatsNode.isNull())
		return;

	// TODO 0.6.6: by tag does not work, this works only if childNodes are "Chat"
	QDomNodeList chatNodes = chatsNode.childNodes();
	//QDomNodeList chatNodes = chatsNode.elementsByTagName("Chat");

	int count = chatNodes.count();

	for (int i = 0; i < count; i++)
	{
		QDomElement chatElement = chatNodes.at(i).toElement();
		if (chatElement.isNull())
			continue;

		StoragePoint *contactStoragePoint = new StoragePoint(configurationStorage, chatElement);
		Chat chat = Chat::loadFromStorage(contactStoragePoint);

		if (chat.details())
			addChat(chat);
	}
}

void ChatManager::store()
{
	ensureLoaded();

	foreach (QList<Chat > list, Chats.values())
		foreach (Chat chat, list)
			chat.store();
}

void ChatManager::addChat(Chat chat)
{
	ensureLoaded();

	emit chatAboutToBeAdded(chat);
	if (!Chats.contains(chat.chatAccount()))
		Chats[chat.chatAccount()] = QList<Chat >();
	Chats[chat.chatAccount()].append(chat);
	emit chatAdded(chat);
}

void ChatManager::removeChat(Chat chat)
{
	ensureLoaded();

	if (!Chats.contains(chat.chatAccount()))
		return;

	emit chatAboutToBeRemoved(chat);
	Chats[chat.chatAccount()].removeOne(chat);
	chat.removeFromStorage();
	emit chatRemoved(chat);
}

QList<Chat > ChatManager::chatsForAccount(Account account)
{
	ensureLoaded();

	if (!Chats.contains(account))
		return QList<Chat>();
	return Chats[account];
}

Chat  ChatManager::byUuid(QUuid uuid)
{
	ensureLoaded();

	foreach (QList<Chat> list, Chats.values())
		foreach (Chat chat, list)
			if (chat.uuid() == uuid)
				return chat;

	return Chat::null;
}
