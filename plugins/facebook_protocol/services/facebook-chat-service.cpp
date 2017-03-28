/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-chat-service.h"
#include "facebook-chat-service.moc"

#include "qfacebook/http/qfacebook-download-threads-result.h"
#include "qfacebook/http/qfacebook-download-unread-messages-result.h"
#include "qfacebook/http/qfacebook-download-unread-threads-result.h"
#include "qfacebook/publish/qfacebook-publish-orca-message-notifications.h"
#include "qfacebook/publish/qfacebook-publish-send-message-response.h"
#include "qfacebook/session/qfacebook-session.h"

#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "message/message-storage.h"
#include "message/message.h"

#include <QtCore/QDateTime>

FacebookChatService::FacebookChatService(Account account, QFacebookSession &facebookSession, QObject *parent)
        : ChatService{account, parent}, m_facebookSession{facebookSession}
{
    connect(&m_facebookSession, &QFacebookSession::threadsReceived, this, &FacebookChatService::threadsReceived);
    connect(
        &m_facebookSession, &QFacebookSession::unreadThreadsReceived, this,
        &FacebookChatService::unreadThreadsReceived);
    connect(
        &m_facebookSession, &QFacebookSession::unreadMessagesReceived, this,
        &FacebookChatService::unreadMessagesReceived);
    connect(
        &m_facebookSession, &QFacebookSession::sendMessageResponseReceived, this,
        &FacebookChatService::sendMessageResponseReceived);
    connect(
        &m_facebookSession, &QFacebookSession::messageNotificationsReceived, this,
        &FacebookChatService::messageNotificationsReceived);

    m_eachSecondTimer.setInterval(1000);
    connect(&m_eachSecondTimer, &QTimer::timeout, this, &FacebookChatService::removeOldUndeliveredMessages);
    connect(&m_eachSecondTimer, &QTimer::timeout, this, &FacebookChatService::removeOldSentMessages);

    m_facebookSession.downloadThreads();
}

FacebookChatService::~FacebookChatService()
{
}

void FacebookChatService::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void FacebookChatService::setChatStorage(ChatStorage *chatStorage)
{
    m_chatStorage = chatStorage;
}

void FacebookChatService::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void FacebookChatService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
    m_formattedStringFactory = formattedStringFactory;
}

void FacebookChatService::setMessageStorage(MessageStorage *messageStorage)
{
    m_messageStorage = messageStorage;
}

int FacebookChatService::maxMessageLength() const
{
    return 10000;
}

bool FacebookChatService::sendMessage(const Message &message)
{
    auto formattedContent = m_formattedStringFactory->fromHtml(message.content());

    FormattedStringPlainTextVisitor plainTextVisitor;
    formattedContent->accept(&plainTextVisitor);

    auto contact = *message.messageChat().contacts().begin();
    auto msgId = m_facebookSession.sendMessage(contact.id().toULongLong(), plainTextVisitor.result().toUtf8());
    m_undeliveredMessages[msgId] = message;
    m_sentMessages.insert(std::make_pair(msgId, QDateTime::currentDateTime()));

    return true;
}

bool FacebookChatService::sendRawMessage(const Chat &chat, const QByteArray &rawMessage)
{
    auto contact = *chat.contacts().begin();
    m_facebookSession.sendMessage(contact.id().toULongLong(), rawMessage);

    return true;
}

void FacebookChatService::leaveChat(const Chat &chat)
{
    chat.setIgnoreAllMessages(true);
}

void FacebookChatService::sendMessageResponseReceived(const QFacebookPublishSendMessageResponse &sendMessageResponse)
{
    auto it = m_undeliveredMessages.find(sendMessageResponse.msgid);
    if (it == std::end(m_undeliveredMessages))
        return;

    if (sendMessageResponse.succeeded)
        it->second.setStatus(MessageStatusDelivered);
    else
        it->second.setStatus(MessageStatusWontDeliver);

    m_undeliveredMessages.erase(it);
}

void FacebookChatService::messageNotificationsReceived(
    const QFacebookPublishOrcaMessageNotifications &orcaMessageNotifications)
{
    auto sentMessageIt = m_sentMessages.find(orcaMessageNotifications.offlineThreadingId);
    if (sentMessageIt != std::end(m_sentMessages))
        return;

    auto contact =
        m_contactManager->byId(account(), QString::number(orcaMessageNotifications.otherUserFbid), ActionCreateAndAdd);
    auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
    auto message = m_messageStorage->create();
    message.setMessageChat(chat);
    message.setType(
        orcaMessageNotifications.otherUserFbid == orcaMessageNotifications.senderFbid ? MessageType::MessageTypeReceived
                                                                                      : MessageType::MessageTypeSent);
    message.setMessageSender(
        orcaMessageNotifications.otherUserFbid == orcaMessageNotifications.senderFbid ? contact
                                                                                      : account().accountContact());

    auto sendDate = QDateTime::fromMSecsSinceEpoch(orcaMessageNotifications.timestamp);
    message.setSendDate(sendDate.toLocalTime());
    message.setReceiveDate(QDateTime::currentDateTime());
    message.setContent(normalizeHtml(plainToHtml(orcaMessageNotifications.body)));

    if (orcaMessageNotifications.otherUserFbid == orcaMessageNotifications.senderFbid)
        emit messageReceived(message);
    else
        emit messageSent(message);

    m_facebookSession.markRead(orcaMessageNotifications.otherUserFbid, m_syncSequenceId);
}

void FacebookChatService::removeOldUndeliveredMessages()
{
    auto now = QDateTime::currentDateTime();
    auto it = m_undeliveredMessages.begin();
    auto removedMessages = std::vector<Message>{};

    while (it != m_undeliveredMessages.end())
        if (it->second.sendDate().addSecs(60) < now)
        {
            removedMessages.push_back(it->second);
            it = m_undeliveredMessages.erase(it);
        }
        else
            ++it;

    for (auto const &message : removedMessages)
    {
        message.setStatus(MessageStatusWontDeliver);
        emit sentMessageStatusChanged(message);
    }
}

void FacebookChatService::removeOldSentMessages()
{
    auto now = QDateTime::currentDateTime();
    auto it = m_sentMessages.begin();

    while (it != m_sentMessages.end())
        if (it->second.addSecs(60) < now)
            it = m_sentMessages.erase(it);
        else
            ++it;
}

void FacebookChatService::threadsReceived(const QFacebookDownloadThreadsResult &result)
{
    m_syncSequenceId = result.syncSequenceId;
    if (result.unreadCount > 0)
        m_facebookSession.downloadUnreadThreads(result.unreadCount);
}

void FacebookChatService::unreadThreadsReceived(const QFacebookDownloadUnreadThreadsResult &result)
{
    for (auto const &unreadThread : result.unreadThreads)
        if (unreadThread.unreadCount > 0)
            m_facebookSession.downloadUnreadMessages(unreadThread.uid, unreadThread.unreadCount);
}

void FacebookChatService::unreadMessagesReceived(const QFacebookDownloadUnreadMessagesResult &result)
{
    if (result.unreadMessages.empty())
        return;

    for (auto const &unreadMessage : result.unreadMessages)
        unreadMessageReceived(unreadMessage);

    m_facebookSession.markRead(result.unreadMessages[0].senderUid, m_syncSequenceId);
}

void FacebookChatService::unreadMessageReceived(const QFacebookDownloadUnreadMessageResult &result)
{
    auto contact = m_contactManager->byId(account(), QString::number(result.senderUid), ActionCreateAndAdd);
    auto chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
    auto message = m_messageStorage->create();
    message.setMessageChat(chat);
    message.setType(MessageType::MessageTypeReceived);
    message.setMessageSender(contact);
    message.setSendDate(result.timestamp);
    message.setReceiveDate(QDateTime::currentDateTime());
    message.setContent(normalizeHtml(plainToHtml(result.text)));

    emit messageReceived(message);
}
