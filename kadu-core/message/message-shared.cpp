/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "message/message.h"
#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message-manager.h"
#include "message/unread-message-repository.h"
#include "misc/change-notifier.h"

#include "message-shared.h"

MessageShared::MessageShared(const QUuid &uuid) : Shared(uuid), Status(MessageStatusUnknown), Type(MessageTypeUnknown)
{
    MessageChat = new Chat();
    MessageSender = new Contact();

    connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

MessageShared::~MessageShared()
{
    ref.ref();

    delete MessageSender;
    delete MessageChat;
}

void MessageShared::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void MessageShared::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void MessageShared::setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository)
{
    m_unreadMessageRepository = unreadMessageRepository;
}

StorableObject *MessageShared::storageParent()
{
    return m_unreadMessageRepository;
}

QString MessageShared::storageNodeName()
{
    return QStringLiteral("Message");
}

void MessageShared::load()
{
    if (!isValidStorage())
        return;

    Shared::load();

    *MessageChat = m_chatManager->byUuid(loadValue<QString>("Chat"));
    *MessageSender = m_contactManager->byUuid(loadValue<QString>("Sender"));
    Content = normalizeHtml(HtmlString{loadValue<QString>("Content")});
    ReceiveDate = loadValue<QDateTime>("ReceiveDate");
    SendDate = loadValue<QDateTime>("SendDate");
    Status = (MessageStatus)loadValue<int>("Status");
    Type = (MessageType)loadValue<int>("Type");
    Id = loadValue<QString>("Id");
}

void MessageShared::store()
{
    if (!isValidStorage())
        return;

    Shared::store();

    storeValue("Chat", MessageChat->uuid().toString());
    storeValue("Sender", MessageSender->uuid().toString());
    storeValue("Content", Content.string());
    storeValue("ReceiveDate", ReceiveDate);
    storeValue("SendDate", SendDate);
    storeValue("Status", (int)Status);
    storeValue("Type", (int)Type);
    storeValue("Id", Id);
}

bool MessageShared::shouldStore()
{
    ensureLoaded();

    // only store pending messages
    // all other messages are stored by history plugin
    return UuidStorableObject::shouldStore() && !MessageSender->uuid().isNull() && !MessageChat->uuid().isNull();
}

void MessageShared::setStatus(MessageStatus status)
{
    ensureLoaded();

    if (status != Status)
    {
        MessageStatus oldStatus = Status;
        Status = status;
        changeNotifier().notify();
        emit statusChanged(oldStatus);
    }
}

KaduShared_PropertyPtrDefCRW(MessageShared, Chat, messageChat, MessageChat)
    KaduShared_PropertyPtrDefCRW(MessageShared, Contact, messageSender, MessageSender)

#include "moc_message-shared.cpp"
