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

		addChat(chat);
	}
}

void ChatManager::store()
{
	ensureLoaded();

	foreach (Chat chat, AllChats)
		chat.store();
}

void ChatManager::registerChat(Chat chat)
{
	if (Chats.contains(chat))
		return;

	emit chatAboutToBeAdded(chat);
	Chats.append(chat);
	emit chatAdded(chat);
}

void ChatManager::unregisterChat(Chat chat)
{
	if (!Chats.contains(chat))
		return;

	emit chatAboutToBeRemoved(chat);
	chat.removeFromStorage();
	emit chatRemoved(chat);
}

void ChatManager::addChat(Chat chat)
{
	ensureLoaded();

	if (AllChats.contains(chat))
		return;

	connect(chat, SIGNAL(chatTypeLoaded()), this, SLOT(chatTypeLoaded()));
	connect(chat, SIGNAL(chatTypeUnloaded()), this, SLOT(chatTypeUnloaded()));

	AllChats.append(chat);
	if (chat.details())
		registerChat(chat);
}

void ChatManager::removeChat(Chat chat)
{
	ensureLoaded();

	if (!AllChats.contains(chat))
		return;

	disconnect(chat, SIGNAL(chatTypeLoaded()), this, SLOT(chatTypeLoaded()));
	disconnect(chat, SIGNAL(chatTypeUnloaded()), this, SLOT(chatTypeUnloaded()));

	AllChats.removeAll(chat);
	if (chat.details())
		unregisterChat(chat);
}

void ChatManager::chatTypeLoaded()
{
	Chat chat(sender());
	if (!chat.isNull())
		registerChat(chat);
}

void ChatManager::chatTypeUnloaded()
{
	Chat chat(sender());
	if (!chat.isNull())
		unregisterChat(chat);
}

QList<Chat> ChatManager::chats()
{
	return Chats;
}

Chat  ChatManager::byUuid(QUuid uuid)
{
	ensureLoaded();

	foreach (Chat chat, Chats)
		if (chat.uuid() == uuid)
			return chat;

	return Chat::null;
}
