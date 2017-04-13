/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "jabber-account-data.h"
#include "services/jabber-account-avatar-service.h"
#include "services/jabber-contact-personal-info-service.h"
#include "services/jabber-file-transfer-service.h"
#include "services/jabber-personal-info-service.h"
#include "services/jabber-subscription-service.h"

#include "protocols/protocol.h"
#include "protocols/services/chat-service.h"

#include <injeqt/injeqt.h>
#include <qxmpp/QXmppClient.h>

class JabberChangePasswordService;
class JabberChatService;
class JabberChatStateService;
class JabberContactAvatarService;
class JabberErrorService;
class JabberOpenChatWithRunner;
class JabberPresenceService;
class JabberRegisterExtension;
class JabberResourceService;
class JabberRoomChatService;
class JabberRosterExtension;
class JabberStreamDebugService;
class PluginInjectedFactory;
class SystemInfo;
class VersionService;

class AggregatedAccountAvatarService;
class AggregatedContactAvatarService;
class ChatServiceRepository;
class ChatStateServiceRepository;

class QXmppClient;
class QXmppMucManager;
class QXmppTransferManager;

class JabberProtocol : public Protocol
{
    Q_OBJECT

public:
    explicit JabberProtocol(Account account, ProtocolFactory *factory);
    virtual ~JabberProtocol();

    virtual QString statusPixmapPath() override;
    virtual bool contactsListReadOnly() override
    {
        return false;
    }

    virtual ContactPersonalInfoService *contactPersonalInfoService() override
    {
        return m_contactPersonalInfoService;
    }
    virtual FileTransferService *fileTransferService() override
    {
        return m_fileTransferService;
    }
    virtual PersonalInfoService *personalInfoService() override
    {
        return m_personalInfoService;
    }
    virtual SubscriptionService *subscriptionService() override
    {
        return m_subscriptionService;
    }
    virtual JabberStreamDebugService *streamDebugService()
    {
        return m_streamDebugService;
    }
    virtual JabberVCardService *vcardService()
    {
        return m_vcardService;
    }

    JabberChangePasswordService *changePasswordService() const;

signals:
    void userStatusChangeIgnored(Buddy);

protected:
    virtual void login() override;
    virtual void logout() override;
    virtual void sendStatusToServer() override;

    virtual void changePrivateMode() override;

private:
    QPointer<AggregatedAccountAvatarService> m_aggregatedAccountAvatarService;
    QPointer<AggregatedContactAvatarService> m_aggregatedContactAvatarService;
    QPointer<ChatServiceRepository> m_chatServiceRepository;
    QPointer<ChatStateServiceRepository> m_chatStateServiceRepository;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<SystemInfo> m_systemInfo;
    QPointer<VersionService> m_versionService;

    JabberAccountAvatarService *m_accountAvatarService;
    JabberChatService *m_chatService;
    JabberChatStateService *m_chatStateService;
    JabberChangePasswordService *m_changePasswordService;
    JabberContactAvatarService *m_contactAvatarService;
    JabberContactPersonalInfoService *m_contactPersonalInfoService;
    JabberErrorService *m_errorService;
    JabberFileTransferService *m_fileTransferService;
    JabberPersonalInfoService *m_personalInfoService;
    JabberSubscriptionService *m_subscriptionService;
    JabberPresenceService *m_presenceService;
    JabberRoomChatService *m_roomChatService;
    JabberStreamDebugService *m_streamDebugService;
    JabberVCardService *m_vcardService;
    JabberResourceService *m_resourceService;
    JabberOpenChatWithRunner *m_jabberOpenChatWithRunner;

    QXmppClient *m_client;
    std::unique_ptr<JabberRegisterExtension> m_registerExtension;
    std::unique_ptr<JabberRosterExtension> m_rosterExtension;
    std::unique_ptr<QXmppMucManager> m_mucManager;
    std::unique_ptr<QXmppTransferManager> m_transferManager;

    bool m_contactsListReadOnly;

private slots:
    INJEQT_SET void setAggregatedAccountAvatarService(AggregatedAccountAvatarService *aggregatedAccountAvatarService);
    INJEQT_SET void setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService);
    INJEQT_SET void setChatServiceRepository(ChatServiceRepository *chatServiceRepository);
    INJEQT_SET void setChatStateServiceRepository(ChatStateServiceRepository *chatStateServiceRepository);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setSystemInfo(SystemInfo *systemInfo);
    INJEQT_SET void setVersionService(VersionService *versionService);
    INJEQT_INIT void init();
    INJEQT_DONE void done();

    void connectedToServer();
    void disconenctedFromServer();
    void error(QXmppClient::Error error);

    void rosterReady();

    void updatePresence();
    void presenceReceived(const QXmppPresence &presence);
};
