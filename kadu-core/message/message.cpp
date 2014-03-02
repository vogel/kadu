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

bool sameMessage(const Message &left, const Message &right)
{
	if (left.isNull() && right.isNull())
		return true;

	if (left.isNull() || right.isNull()) // one is null, second one is not
		return false;

	if (left.type() != right.type())
		return false;

	// In our SQL history we store datetime with accuracy to one second,
	// while for received XMPP messages we have a millisecond accuracy.
	// So to have proper results, we need to truncate those additional milliseconds.
	if (left.sendDate().toTime_t() != right.sendDate().toTime_t())
		return false;

	if (left.messageChat() != right.messageChat())
		return false;

	if (left.messageSender() != right.messageSender())
		return false;

	if (*left.content() != *right.content())
		return false;

	return true;
}
