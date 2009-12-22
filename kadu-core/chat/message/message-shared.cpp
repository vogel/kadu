/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/message/pending-messages-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"

#include "message-shared.h"

MessageShared * MessageShared::loadFromStorage(StoragePoint *messageStoragePoint)
{
	MessageShared *result = new MessageShared();
	result->setStorage(messageStoragePoint);
	return result;
}

MessageShared::MessageShared(QUuid uuid) :
		Shared(uuid), Status(Message::StatusUnknown), Type(Message::TypeUnknown), Pending(false)
{
}

MessageShared::~MessageShared()
{
}

StorableObject * MessageShared::storageParent()
{
	return PendingMessagesManager::instance();
}

QString MessageShared::storageNodeName()
{
	return QLatin1String("Message");
}

void MessageShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	MessageChat = ChatManager::instance()->byUuid(loadValue<QString>("Chat"), false);
	MessageSender = ContactManager::instance()->byUuid(loadValue<QString>("Sender"), false);
	Content = loadValue<QString>("Content");
	ReceiveDate = loadValue<QDateTime>("ReceiveDate");
	SendDate = loadValue<QDateTime>("SendDate");
	Status = (Message::Status)loadValue<int>("Status");
	Type = (Message::Type)loadValue<int>("Type");
	Pending = (Message::Type)loadValue<bool>("Pending");
	Id = loadValue<int>("Id");
}

void MessageShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Chat", MessageChat.uuid().toString());
	storeValue("Sender", messageSender().uuid().toString());
	storeValue("Content", Content);
	storeValue("ReceiveDate", ReceiveDate);
	storeValue("SendDate", SendDate);
	storeValue("Status", (int)Status);
	storeValue("Type", (int)Type);
	storeValue("Pending", Pending);
	storeValue("Id", Id);
}

bool MessageShared::shouldStore()
{
	// only store pending messages
	// all other messages are stored by history module
	return UuidStorableObject::shouldStore()
			&& !MessageSender.uuid().isNull()
			&& !MessageChat.uuid().isNull()
			&& Pending;
}

void MessageShared::emitUpdated()
{
	emit updated();
}

void MessageShared::setStatus(Message::Status status)
{
	if (status != Status)
	{
		Status = status;
		emit statusChanged(Status);
	}
}
