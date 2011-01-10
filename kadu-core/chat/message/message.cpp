/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "chat/message/message-shared.h"

#include "message.h"

KaduSharedBaseClassImpl(Message)

Message Message::null;

Message Message::create()
{
	return new MessageShared();
}

Message Message::loadStubFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint)
{
	return MessageShared::loadStubFromStorage(messageStoragePoint);
}

Message Message::loadFromStorage(const QSharedPointer<StoragePoint> &messageStoragePoint)
{
	return MessageShared::loadFromStorage(messageStoragePoint);
}

Message::Message()
{
}

Message::Message(MessageShared *data) :
		SharedBase<MessageShared>(data)
{
}

Message::Message(QObject *data)
{
	MessageShared *shared = dynamic_cast<MessageShared *>(data);
	if (shared)
		setData(shared);
}

Message::Message(const Message &copy) :
		SharedBase<MessageShared>(copy)
{
}

Message::~Message()
{
}

KaduSharedBase_PropertyDef(Message, Chat, messageChat, MessageChat, Chat::null)
KaduSharedBase_PropertyDef(Message, Contact, messageSender, MessageSender, Contact::null)
KaduSharedBase_PropertyDef(Message, QString, content, Content, QString())
KaduSharedBase_PropertyDef(Message, QDateTime, receiveDate, ReceiveDate, QDateTime())
KaduSharedBase_PropertyDef(Message, QDateTime, sendDate, SendDate, QDateTime())
KaduSharedBase_PropertyDef(Message, Message::Status, status, Status, Message::StatusUnknown)
KaduSharedBase_PropertyDef(Message, Message::Type, type, Type, Message::TypeUnknown)
KaduSharedBase_PropertyBoolDef(Message, Pending, false)
KaduSharedBase_PropertyDef(Message, int, id, Id, -1)
