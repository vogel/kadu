/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "identity-status-container.h"
#include "identity-status-container.moc"

#include "accounts/account-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity-shared.h"
#include "status/status-configuration-holder.h"
#include "status/status-type-manager.h"

IdentityStatusContainer::IdentityStatusContainer(IdentityShared *identityShared)
        : StorableStatusContainer{identityShared}, m_identityShared{identityShared}
{
}

IdentityStatusContainer::~IdentityStatusContainer()
{
}

void IdentityStatusContainer::setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder)
{
    m_statusConfigurationHolder = statusConfigurationHolder;
}

QString IdentityStatusContainer::statusContainerName()
{
    return m_identityShared->name();
}

void IdentityStatusContainer::setStatus(Status status, StatusChangeSource source)
{
    m_lastSetStatus = status;
    for (auto const &account : m_identityShared->accounts())
        if (account)
            account.statusContainer()->setStatus(status, source);
}

Status IdentityStatusContainer::status()
{
    auto account = AccountManager::bestAccount(m_identityShared->accounts());
    return account ? account.statusContainer()->status() : Status();
}

bool IdentityStatusContainer::isStatusSettingInProgress()
{
    auto account = AccountManager::bestAccount(m_identityShared->accounts());
    return account ? account.statusContainer()->isStatusSettingInProgress() : false;
}

KaduIcon IdentityStatusContainer::statusIcon()
{
    return statusIcon(status());
}

KaduIcon IdentityStatusContainer::statusIcon(const Status &status)
{
    QSet<QString> protocols;
    for (auto const &account : m_identityShared->accounts())
        protocols.insert(account.protocolName());

    if (protocols.count() > 1)
        return statusTypeManager()->statusIcon("common", status);

    auto account = AccountManager::bestAccount(m_identityShared->accounts());
    return account ? account.statusContainer()->statusIcon(status) : KaduIcon();
}

QList<StatusType> IdentityStatusContainer::supportedStatusTypes()
{
    auto account = AccountManager::bestAccount(m_identityShared->accounts());
    return account ? account.statusContainer()->supportedStatusTypes() : QList<StatusType>();
}

int IdentityStatusContainer::maxDescriptionLength()
{
    auto account = AccountManager::bestAccount(m_identityShared->accounts());
    return account ? account.statusContainer()->maxDescriptionLength() : -1;
}

void IdentityStatusContainer::addAccount(const Account &account)
{
    connect(
        account.statusContainer(), SIGNAL(statusUpdated(StatusContainer *)), this,
        SIGNAL(statusUpdated(StatusContainer *)));
    if (m_statusConfigurationHolder->isSetStatusPerIdentity())
        account.statusContainer()->setStatus(m_lastSetStatus, SourceStatusChanger);

    emit statusUpdated(this);
}

void IdentityStatusContainer::removeAccount(const Account &account)
{
    disconnect(account.statusContainer(), nullptr, this, nullptr);
    emit statusUpdated(this);
}
