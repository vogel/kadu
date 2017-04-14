/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-style-preview.h"
#include "chat-style-preview.moc"

#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-storage.h"
#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "chat/chat-details-contact.h"
#include "chat/chat-storage.h"
#include "contacts/contact-storage.h"
#include "core/myself.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "message/message-storage.h"
#include "widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "widgets/webkit-messages-view/webkit-messages-view.h"

#include <QtWidgets/QHBoxLayout>

ChatStylePreview::ChatStylePreview(QWidget *parent) : QFrame{parent}
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setFixedHeight(250);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

ChatStylePreview::~ChatStylePreview()
{
}

void ChatStylePreview::setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager)
{
    m_buddyPreferredManager = buddyPreferredManager;
}

void ChatStylePreview::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
    m_chatConfigurationHolder = chatConfigurationHolder;
}

void ChatStylePreview::setChatStorage(ChatStorage *chatStorage)
{
    m_chatStorage = chatStorage;
}

void ChatStylePreview::setContactStorage(ContactStorage *contactStorage)
{
    m_contactStorage = contactStorage;
}

void ChatStylePreview::setBuddyStorage(BuddyStorage *buddyStorage)
{
    m_buddyStorage = buddyStorage;
}

void ChatStylePreview::setMessageStorage(MessageStorage *messageStorage)
{
    m_messageStorage = messageStorage;
}

void ChatStylePreview::setMyself(Myself *myself)
{
    m_myself = myself;
}

void ChatStylePreview::setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory)
{
    m_webkitMessagesViewFactory = webkitMessagesViewFactory;
}

void ChatStylePreview::init()
{
    auto layout = make_owned<QHBoxLayout>(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_view = preparePreview();
    layout->addWidget(m_view.get());

    configurationUpdated();
}

void ChatStylePreview::setRendererFactory(std::unique_ptr<ChatStyleRendererFactory> rendererFactory)
{
    m_view.get()->setChatStyleRendererFactory(std::move(rendererFactory));
}

owned_qptr<WebkitMessagesView> ChatStylePreview::preparePreview()
{
    auto example = m_buddyStorage->create();

    auto chat = m_chatStorage->create("Contact");

    auto details = static_cast<ChatDetailsContact *>(chat.details());
    details->setState(StorableObject::StateNew);
    details->setContact(m_buddyPreferredManager->preferredContact(example));

    auto buddy = m_buddyStorage->create();
    buddy.setDisplay(m_myself->buddy().display());
    auto contact = m_contactStorage->create();
    contact.setId("id@network");
    contact.setOwnerBuddy(buddy);

    auto sentMessage = m_messageStorage->create();
    sentMessage.setMessageChat(chat);
    sentMessage.setType(MessageTypeSent);
    sentMessage.setMessageSender(contact);
    sentMessage.setContent(normalizeHtml(plainToHtml(tr("Your message"))));
    sentMessage.setReceiveDate(QDateTime::currentDateTime());
    sentMessage.setSendDate(QDateTime::currentDateTime());

    auto receivedMessage = m_messageStorage->create();
    receivedMessage.setMessageChat(chat);
    receivedMessage.setType(MessageTypeReceived);
    receivedMessage.setMessageSender(m_buddyPreferredManager->preferredContact(example));
    receivedMessage.setContent(normalizeHtml(plainToHtml(tr("Message from Your friend"))));
    receivedMessage.setReceiveDate(QDateTime::currentDateTime());
    receivedMessage.setSendDate(QDateTime::currentDateTime());

    auto result = m_webkitMessagesViewFactory->createWebkitMessagesView(chat, false, this);
    result->add(sentMessage);
    result->add(receivedMessage);
    return result;
}

void ChatStylePreview::configurationUpdated()
{
    m_view->setUserFont(
        m_chatConfigurationHolder->chatFont().toString(), m_chatConfigurationHolder->forceCustomChatFont());
}
