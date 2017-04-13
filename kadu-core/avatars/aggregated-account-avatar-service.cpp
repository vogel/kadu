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

#include "aggregated-account-avatar-service.h"
#include "aggregated-account-avatar-service.moc"

#include "avatars/account-avatar-service.h"

#include <QtGui/QPixmap>
#include <cassert>

void AggregatedAccountAvatarService::upload(const Account &account, QPixmap avatar) const
{
    auto service = m_services.find(account);
    if (service == std::end(m_services))
    {
        emit finished(account, false);
        return;
    }

    service->second->upload(std::move(avatar));
}

void AggregatedAccountAvatarService::add(AccountAvatarService *service)
{
    assert(m_services.find(service->account()) == std::end(m_services));

    m_services.insert(std::make_pair(service->account(), service));
    connect(service, &AccountAvatarService::finished, this, &AggregatedAccountAvatarService::subFinished);

    emit availabilityChanged();
}

void AggregatedAccountAvatarService::remove(AccountAvatarService *service)
{
    auto it = m_services.find(service->account());
    assert(it != std::end(m_services));

    disconnect(service, &AccountAvatarService::finished, this, &AggregatedAccountAvatarService::subFinished);

    m_services.erase(it);

    emit availabilityChanged();
}

AggregatedAccountAvatarService::Availability AggregatedAccountAvatarService::availability(const Account &account) const
{
    auto it = m_services.find(account);
    if (it == std::end(m_services))
        return Availability::None;
    return it->second->canRemove() ? Availability::Full : Availability::UploadOnly;
}

void AggregatedAccountAvatarService::subFinished(bool ok)
{
    auto account = static_cast<AccountAvatarService *>(sender())->account();
    emit finished(account, ok);
}
