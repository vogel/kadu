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

#include "accounts/account.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>
#include <memory>

class FacebookChatService;
class FacebookRosterService;
class QFacebookSession;

class ChatServiceRepository;
class ContactManager;
class PluginInjectedFactory;

class FacebookServices : public QObject
{
    Q_OBJECT

public:
    explicit FacebookServices(Account account, std::unique_ptr<QFacebookSession> session);
    virtual ~FacebookServices();

private:
    QPointer<ChatServiceRepository> m_chatServiceRepository;
    QPointer<ContactManager> m_contactManager;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

    Account m_account;
    std::unique_ptr<QFacebookSession> m_session;
    std::unique_ptr<FacebookChatService> m_chatService;
    std::unique_ptr<FacebookRosterService> m_rosterService;

private slots:
    INJEQT_SET void setChatServiceRepository(ChatServiceRepository *chatServiceRepository);
    INJEQT_SET void setContactManager(ContactManager *contactManager);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_INIT void init();
};
