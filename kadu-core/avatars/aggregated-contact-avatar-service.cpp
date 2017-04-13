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

#include "aggregated-contact-avatar-service.h"
#include "aggregated-contact-avatar-service.moc"

#include "avatars/contact-avatar-global-id.h"
#include "avatars/contact-avatar-id.h"
#include "avatars/contact-avatar-service.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact-id.h"

#include <cassert>

void AggregatedContactAvatarService::download(const ContactAvatarGlobalId &id) const
{
    auto service = m_services.find(id.contact.account);
    if (service == std::end(m_services))
        return;

    service->second->download({id.contact.id, id.id});
}

void AggregatedContactAvatarService::add(ContactAvatarService *service)
{
    assert(m_services.find(service->account()) == std::end(m_services));

    m_services.insert(std::make_pair(service->account(), service));
    connect(service, &ContactAvatarService::available, this, &AggregatedContactAvatarService::subAvailable);
    connect(service, &ContactAvatarService::downloaded, this, &AggregatedContactAvatarService::subDownloaded);
    connect(service, &ContactAvatarService::removed, this, &AggregatedContactAvatarService::subRemoved);
}

void AggregatedContactAvatarService::remove(ContactAvatarService *service)
{
    auto it = m_services.find(service->account());
    assert(it != std::end(m_services));

    disconnect(service, &ContactAvatarService::available, this, &AggregatedContactAvatarService::subAvailable);
    disconnect(service, &ContactAvatarService::downloaded, this, &AggregatedContactAvatarService::subDownloaded);
    disconnect(service, &ContactAvatarService::removed, this, &AggregatedContactAvatarService::subRemoved);

    m_services.erase(it);
}

void AggregatedContactAvatarService::subAvailable(const ContactAvatarId &id)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit available({{account, id.contact}, id.id});
}

void AggregatedContactAvatarService::subDownloaded(const ContactAvatarId &id, const QByteArray &content)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit downloaded({{account, id.contact}, id.id}, content);
}

void AggregatedContactAvatarService::subRemoved(const ContactId &id)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit removed({account, id});
}
