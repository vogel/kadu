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
    auto service = m_contactAvatarServices.find(id.contact.account);
    if (service == std::end(m_contactAvatarServices))
        return;

    service->second->download({id.contact.id, id.id});
}

void AggregatedContactAvatarService::addContactAvatarService(ContactAvatarService *contactAvatarService)
{
    assert(m_contactAvatarServices.find(contactAvatarService->account()) == std::end(m_contactAvatarServices));

    m_contactAvatarServices.insert(std::make_pair(contactAvatarService->account(), contactAvatarService));
    connect(
        contactAvatarService, &ContactAvatarService::available, this, &AggregatedContactAvatarService::subAvailable);
    connect(
        contactAvatarService, &ContactAvatarService::downloaded, this, &AggregatedContactAvatarService::subDownloaded);
    connect(contactAvatarService, &ContactAvatarService::removed, this, &AggregatedContactAvatarService::subRemoved);
}

void AggregatedContactAvatarService::removeContactAvatarService(ContactAvatarService *contactAvatarService)
{
    auto it = m_contactAvatarServices.find(contactAvatarService->account());
    assert(it != std::end(m_contactAvatarServices));

    disconnect(
        contactAvatarService, &ContactAvatarService::available, this, &AggregatedContactAvatarService::subAvailable);
    disconnect(
        contactAvatarService, &ContactAvatarService::downloaded, this, &AggregatedContactAvatarService::subDownloaded);
    disconnect(contactAvatarService, &ContactAvatarService::removed, this, &AggregatedContactAvatarService::subRemoved);

    m_contactAvatarServices.erase(it);
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
