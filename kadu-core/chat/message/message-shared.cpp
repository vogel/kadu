/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "chat/message/pending-messages-manager.h"
#include "chat/chat-manager.h"
#include "contacts/contact-manager.h"

#include "message-shared.h"

MessageShared * MessageShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint)
{
	MessageShared *result = loadFromStorage(messageStoragePoint);
	result->loadStub();
	return result;
}

MessageShared * MessageShared::loadFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint)
{
	MessageShared *result = new MessageShared();
	result->setStorage(messageStoragePoint);
	return result;
}

MessageShared::MessageShared(QUuid uuid) :
		Shared(uuid), Status(Message::StatusUnknown), Type(Message::TypeUnknown), Pending(false), Id(-1)
{
}

MessageShared::~MessageShared()
{
	ref.ref();
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

	MessageChat = ChatManager::instance()->byUuid(loadValue<QString>("Chat"));
	MessageSender = ContactManager::instance()->byUuid(loadValue<QString>("Sender"));
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
	ensureLoaded();

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
	ensureLoaded();

	if (status != Status)
	{
		Status = status;
		dataUpdated();
		emit statusChanged(Status);
	}
}
