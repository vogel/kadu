/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"

#include "message.h"

KaduSharedBaseClassImpl(Message)

Message Message::null;

Message::Message()
{
}

Message::Message(MessageShared *data) :
		SharedBase<MessageShared>(data)
{
}

Message::Message(QObject *data)
{
	MessageShared *shared = qobject_cast<MessageShared *>(data);
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

KaduSharedBase_PropertyDefCRW(Message, Chat, messageChat, MessageChat, Chat::null)
KaduSharedBase_PropertyDefCRW(Message, Contact, messageSender, MessageSender, Contact::null)
KaduSharedBase_PropertyDefCRW(Message, QString, htmlContent, HtmlContent, QString())
KaduSharedBase_PropertyDefCRW(Message, QDateTime, receiveDate, ReceiveDate, QDateTime())
KaduSharedBase_PropertyDefCRW(Message, QDateTime, sendDate, SendDate, QDateTime())
KaduSharedBase_PropertyDef(Message, MessageStatus, status, Status, MessageStatusUnknown)
KaduSharedBase_PropertyDef(Message, MessageType, type, Type, MessageTypeUnknown)
KaduSharedBase_PropertyDefCRW(Message, QString, id, Id, QString())
