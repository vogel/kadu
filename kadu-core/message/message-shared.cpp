/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "message/message-manager.h"
#include "message/message.h"

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

MessageShared::MessageShared(const QUuid &uuid) :
		Shared(uuid), Status(MessageStatusUnknown), Type(MessageTypeUnknown), Pending(false)
{
	MessageChat = new Chat();
	MessageSender = new Contact();
}

MessageShared::~MessageShared()
{
	ref.ref();

	delete MessageSender;
	delete MessageChat;
}

StorableObject * MessageShared::storageParent()
{
	return MessageManager::instance();
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

	*MessageChat = ChatManager::instance()->byUuid(loadValue<QString>("Chat"));
	*MessageSender = ContactManager::instance()->byUuid(loadValue<QString>("Sender"));
	Content = loadValue<QString>("Content");
	ReceiveDate = loadValue<QDateTime>("ReceiveDate");
	SendDate = loadValue<QDateTime>("SendDate");
	Status = (MessageStatus)loadValue<int>("Status");
	Type = (MessageType)loadValue<int>("Type");
	Pending = (MessageType)loadValue<bool>("Pending");
	Id = loadValue<QString>("Id");
}

void MessageShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("Chat", MessageChat->uuid().toString());
	storeValue("Sender", MessageSender->uuid().toString());
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
	// all other messages are stored by history plugin
	return UuidStorableObject::shouldStore()
			&& !MessageSender->uuid().isNull()
			&& !MessageChat->uuid().isNull()
			&& Pending;
}

void MessageShared::emitUpdated()
{
	emit updated();
}

void MessageShared::setStatus(MessageStatus status)
{
	ensureLoaded();

	if (status != Status)
	{
		MessageStatus oldStatus = Status;
		Status = status;
		dataUpdated();
		emit statusChanged(oldStatus);
	}
}

KaduShared_PropertyPtrDefCRW(MessageShared, Chat, messageChat, MessageChat)
KaduShared_PropertyPtrDefCRW(MessageShared, Contact, messageSender, MessageSender)
