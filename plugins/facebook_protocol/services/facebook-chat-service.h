/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "qfacebook/qfacebook-msgid.h"
#include "qfacebook/qfacebook-uid.h"

#include "protocols/services/chat-service.h"

#include <QtCore/QTimer>
#include <injeqt/injeqt.h>
#include <map>
#include <set>

class QFacebookSession;
struct QFacebookDownloadThreadsResult;
struct QFacebookDownloadUnreadMessageResult;
struct QFacebookDownloadUnreadMessagesResult;
struct QFacebookDownloadUnreadThreadsResult;
struct QFacebookPublishOrcaMessageNotifications;
struct QFacebookPublishSendMessageResponse;

class ChatManager;
class ChatStorage;
class ContactManager;
class FormattedStringFactory;
class MessageStorage;

class FacebookChatService : public ChatService
{
    Q_OBJECT

public:
    explicit FacebookChatService(Account account, QFacebookSession &facebookSession, QObject *parent = nullptr);
    virtual ~FacebookChatService();

    virtual int maxMessageLength() const override;

public slots:
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
    INJEQT_SET void setContactManager(ContactManager *contactManager);
    INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);
    INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);

    virtual bool sendMessage(const Message &message) override;
    virtual bool sendRawMessage(const Chat &chat, const QByteArray &rawMessage) override;
    virtual void leaveChat(const Chat &chat) override;

private:
    QPointer<ChatManager> m_chatManager;
    QPointer<ChatStorage> m_chatStorage;
    QPointer<ContactManager> m_contactManager;
    QPointer<FormattedStringFactory> m_formattedStringFactory;
    QPointer<MessageStorage> m_messageStorage;

    QFacebookSession &m_facebookSession;
    std::map<QFacebookMsgId, Message> m_undeliveredMessages;
    std::map<QFacebookMsgId, QDateTime> m_sentMessages;
    QTimer m_eachSecondTimer;
    int m_syncSequenceId{-1};

    void sendMessageResponseReceived(const QFacebookPublishSendMessageResponse &sendMessageResponse);
    void messageNotificationsReceived(const QFacebookPublishOrcaMessageNotifications &orcaMessageNotifications);
    void removeOldUndeliveredMessages();
    void removeOldSentMessages();

    void threadsReceived(const QFacebookDownloadThreadsResult &result);
    void unreadThreadsReceived(const QFacebookDownloadUnreadThreadsResult &result);
    void unreadMessagesReceived(const QFacebookDownloadUnreadMessagesResult &result);
    void unreadMessageReceived(const QFacebookDownloadUnreadMessageResult &result);
};
