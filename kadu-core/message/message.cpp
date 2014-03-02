/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "contacts/contact.h"
#include "formatted-string/formatted-string.h"

#include "message.h"

KaduSharedBaseClassImpl(Message)

Message Message::null;

Message Message::create()
{
	return new MessageShared();
}

Message Message::loadStubFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint)
{
	return MessageShared::loadStubFromStorage(messageStoragePoint);
}

Message Message::loadFromStorage(const std::shared_ptr<StoragePoint> &messageStoragePoint)
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

void Message::setContent(std::unique_ptr<FormattedString> &&content) const
{
	if (!isNull())
		data()->setContent(std::move(content));
}

KaduSharedBase_PropertyReadDef(Message, FormattedString *, content, Content, 0)
KaduSharedBase_PropertyReadDef(Message, QString, plainTextContent, PlainTextContent, QString())
KaduSharedBase_PropertyReadDef(Message, QString, htmlContent, HtmlContent, QString())
KaduSharedBase_PropertyDefCRW(Message, QDateTime, receiveDate, ReceiveDate, QDateTime())
KaduSharedBase_PropertyDefCRW(Message, QDateTime, sendDate, SendDate, QDateTime())
KaduSharedBase_PropertyDef(Message, MessageStatus, status, Status, MessageStatusUnknown)
KaduSharedBase_PropertyDef(Message, MessageType, type, Type, MessageTypeUnknown)
KaduSharedBase_PropertyDefCRW(Message, QString, id, Id, QString())
