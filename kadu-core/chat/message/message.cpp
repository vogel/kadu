/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat/message/message-shared.h"

#include "message.h"

KaduSharedBaseClassImpl(Message)

Message Message::null;

Message Message::create()
{
	return new MessageShared();
}

Message Message::loadFromStorage(StoragePoint *messageStoragePoint)
{
	return MessageShared::loadFromStorage(messageStoragePoint);
}

Message::Message()
{
}

Message::Message(MessageShared *data) :
		SharedBase<MessageShared>(data)
{
	data->ref.ref();
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
KaduSharedBase_PropertyDef(Message, QString, content, Content, QString::null)
KaduSharedBase_PropertyDef(Message, QDateTime, receiveDate, ReceiveDate, QDateTime())
KaduSharedBase_PropertyDef(Message, QDateTime, sendDate, SendDate, QDateTime())
KaduSharedBase_PropertyDef(Message, Message::Status, status, Status, Message::StatusUnknown)
KaduSharedBase_PropertyDef(Message, Message::Type, type, Type, Message::TypeUnknown)
KaduSharedBase_PropertyBoolDef(Message, Pending, false)
KaduSharedBase_PropertyDef(Message, int, id, Id, -1)
