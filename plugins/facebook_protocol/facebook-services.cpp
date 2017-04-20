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

#include "facebook-services.h"
#include "facebook-services.moc"

#include "qfacebook/session/qfacebook-session.h"
#include "services/facebook-chat-service.h"
#include "services/facebook-contact-avatar-service.h"
#include "services/facebook-roster-service.h"

#include "avatars/aggregated-contact-avatar-service.h"
#include "avatars/contact-avatar-id.h"
#include "chat/chat-service-repository.h"
#include "contacts/contact-manager.h"
#include "plugin/plugin-injected-factory.h"

FacebookServices::FacebookServices(Account account, std::unique_ptr<QFacebookSession> session)
        : m_account{account}, m_session{std::move(session)}
{
}

FacebookServices::~FacebookServices()
{
    m_aggregatedContactAvatarService->remove(m_contactAvatarService.get());
    m_chatServiceRepository->removeChatService(m_chatService.get());
}

void FacebookServices::setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService)
{
    m_aggregatedContactAvatarService = aggregatedContactAvatarService;
}

void FacebookServices::setChatServiceRepository(ChatServiceRepository *chatServiceRepository)
{
    m_chatServiceRepository = chatServiceRepository;
}

void FacebookServices::setContactManager(ContactManager *contactManager)
{
    m_contactManager = contactManager;
}

void FacebookServices::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void FacebookServices::init()
{
    m_contactAvatarService = m_pluginInjectedFactory->makeUnique<FacebookContactAvatarService>(m_account);
    m_aggregatedContactAvatarService->add(m_contactAvatarService.get());

    m_chatService = m_pluginInjectedFactory->makeUnique<FacebookChatService>(m_account, *m_session);
    m_chatServiceRepository->addChatService(m_chatService.get());

    auto contacts = m_contactManager->contacts(m_account, ContactManager::ExcludeAnonymous);
    m_rosterService = m_pluginInjectedFactory->makeUnique<FacebookRosterService>(
        std::move(contacts), *m_account.protocolHandler(), *m_session);

    connect(
        m_rosterService.get(), &FacebookRosterService::added, m_contactAvatarService.get(),
        &FacebookContactAvatarService::contactAdded);
    connect(
        m_chatService.get(), &FacebookChatService::selfAvatarAvailable, this, &FacebookServices::selfAvatarAvailable);
}

void FacebookServices::selfAvatarAvailable(const QByteArray &path)
{
    m_contactAvatarService->download(ContactAvatarId{{m_account.accountContact().id().toUtf8()}, path});
}
