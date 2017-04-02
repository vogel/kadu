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

#include "avatars/contact-avatar-service.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact-id.h"

#include <cassert>

void AggregatedContactAvatarService::addContactAvatarService(ContactAvatarService *contactAvatarService)
{
    assert(m_contactAvatarServices.find(contactAvatarService->account()) == std::end(m_contactAvatarServices));

    m_contactAvatarServices.insert(std::make_pair(contactAvatarService->account(), contactAvatarService));
    connect(
        contactAvatarService, &ContactAvatarService::avatarAvailable, this,
        &AggregatedContactAvatarService::avatarAvailableTranslator);
    connect(
        contactAvatarService, &ContactAvatarService::avatarDownloaded, this,
        &AggregatedContactAvatarService::avatarDownloadedTranslator);
    connect(
        contactAvatarService, &ContactAvatarService::avatarRemoved, this,
        &AggregatedContactAvatarService::avatarRemovedTranslator);
}

void AggregatedContactAvatarService::removeContactAvatarService(ContactAvatarService *contactAvatarService)
{
    auto it = m_contactAvatarServices.find(contactAvatarService->account());
    assert(it != std::end(m_contactAvatarServices));

    disconnect(
        contactAvatarService, &ContactAvatarService::avatarAvailable, this,
        &AggregatedContactAvatarService::avatarAvailableTranslator);
    disconnect(
        contactAvatarService, &ContactAvatarService::avatarDownloaded, this,
        &AggregatedContactAvatarService::avatarDownloadedTranslator);
    disconnect(
        contactAvatarService, &ContactAvatarService::avatarRemoved, this,
        &AggregatedContactAvatarService::avatarRemovedTranslator);

    m_contactAvatarServices.erase(it);
}

void AggregatedContactAvatarService::avatarAvailableTranslator(const ContactId &contactId, const QByteArray &id)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit avatarAvailable({account, contactId}, id);
}

void AggregatedContactAvatarService::avatarDownloadedTranslator(
    const ContactId &contactId, const QByteArray &id, const QByteArray &content)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit avatarDownloaded({account, contactId}, id, content);
}

void AggregatedContactAvatarService::avatarRemovedTranslator(const ContactId &contactId)
{
    auto account = static_cast<ContactAvatarService *>(sender())->account();
    emit avatarRemoved({account, contactId});
}
