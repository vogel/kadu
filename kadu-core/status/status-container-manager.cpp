/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "protocols/protocol.h"
#include "status/all-accounts-status-container.h"
#include "status/status-configuration-holder.h"
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "status/status-type.h"

#include "status-container-manager.h"

StatusContainerManager::StatusContainerManager(QObject *parent) : StatusContainer(parent), DefaultStatusContainer(0)
{
}

StatusContainerManager::~StatusContainerManager()
{
}

void StatusContainerManager::setAccountManager(AccountManager *accountManager)
{
    m_accountManager = accountManager;
}

void StatusContainerManager::setIdentityManager(IdentityManager *identityManager)
{
    m_identityManager = identityManager;
}

void StatusContainerManager::setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder)
{
    m_statusConfigurationHolder = statusConfigurationHolder;
}

void StatusContainerManager::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
    m_statusTypeManager = statusTypeManager;
}

void StatusContainerManager::init()
{
    if (m_statusConfigurationHolder->isSetStatusPerIdentity())
        triggerAllIdentitiesAdded(m_identityManager);
    else if (m_statusConfigurationHolder->isSetStatusPerAccount())
        triggerAllAccountsAdded(m_accountManager);
    else
        registerStatusContainer(m_allAccountsStatusContainer);

    connect(m_statusConfigurationHolder, SIGNAL(setStatusModeChanged()), this, SLOT(setStatusModeChanged()));
    connect(m_accountManager, SIGNAL(accountUpdated(Account)), this, SLOT(updateIdentities()));
}

void StatusContainerManager::done()
{
    if (m_statusConfigurationHolder)
    {
        if (m_statusConfigurationHolder->isSetStatusPerIdentity())
            triggerAllIdentitiesRemoved(m_identityManager);
        else if (m_statusConfigurationHolder->isSetStatusPerAccount())
            triggerAllAccountsRemoved(m_accountManager);
        else
            unregisterStatusContainer(m_allAccountsStatusContainer);
    }
}

void StatusContainerManager::updateIdentities()
{
    if (!m_statusConfigurationHolder->isSetStatusPerIdentity())
        return;

    foreach (const Identity &identity, m_identityManager->items())
        if (StatusContainers.contains(identity.statusContainer()) && !identity.hasAnyLoadedAccount())
            unregisterStatusContainer(identity.statusContainer());
        else if (!StatusContainers.contains(identity.statusContainer()) && identity.hasAnyLoadedAccount())
            registerStatusContainer(identity.statusContainer());
}

void StatusContainerManager::accountAdded(Account account)
{
    connect(account, SIGNAL(protocolHandlerChanged(Account)), this, SLOT(protocolHandlerChanged(Account)));
    protocolHandlerChanged(account);
}

void StatusContainerManager::accountRemoved(Account account)
{
    disconnect(account, SIGNAL(protocolHandlerChanged(Account)), this, SLOT(protocolHandlerChanged(Account)));
    protocolHandlerChanged(account);
}

void StatusContainerManager::protocolHandlerChanged(Account account)
{
    if (account.protocolHandler())
    {
        if (m_statusConfigurationHolder->isSetStatusPerAccount() &&
            !StatusContainers.contains(account.statusContainer()))
            registerStatusContainer(account.statusContainer());
        if (m_statusConfigurationHolder->isSetStatusPerIdentity() &&
            !StatusContainers.contains(account.accountIdentity().statusContainer()))
            updateIdentities();
    }
    else
    {
        if (m_statusConfigurationHolder->isSetStatusPerAccount() &&
            StatusContainers.contains(account.statusContainer()))
            unregisterStatusContainer(account.statusContainer());
        if (m_statusConfigurationHolder->isSetStatusPerIdentity())
            updateIdentities();
    }
}

void StatusContainerManager::identityAdded(Identity identity)
{
    if (m_statusConfigurationHolder->isSetStatusPerIdentity() &&
        !StatusContainers.contains(identity.statusContainer()) && identity.hasAnyLoadedAccount())
        registerStatusContainer(identity.statusContainer());
}

void StatusContainerManager::identityRemoved(Identity identity)
{
    if (m_statusConfigurationHolder->isSetStatusPerIdentity() && StatusContainers.contains(identity.statusContainer()))
        unregisterStatusContainer(identity.statusContainer());
}

void StatusContainerManager::cleanStatusContainers()
{
    while (!StatusContainers.isEmpty())
        unregisterStatusContainer(StatusContainers.at(0));
}

void StatusContainerManager::addAllAccounts()
{
    foreach (Account account, m_accountManager->items())
        registerStatusContainer(account.statusContainer());
}

void StatusContainerManager::addAllIdentities()
{
    updateIdentities();
}

void StatusContainerManager::setAllAccountsStatusContainer(AllAccountsStatusContainer *allAccountsStatusContainer)
{
    m_allAccountsStatusContainer = allAccountsStatusContainer;
}

void StatusContainerManager::setDefaultStatusContainer(StatusContainer *defaultStatusContainer)
{
    if (defaultStatusContainer == DefaultStatusContainer)
        return;

    if (DefaultStatusContainer)
        disconnect(DefaultStatusContainer, 0, this, 0);

    if (this != defaultStatusContainer)
        DefaultStatusContainer = defaultStatusContainer;
    else
        DefaultStatusContainer = 0;

    if (DefaultStatusContainer)
        connect(
            DefaultStatusContainer, SIGNAL(statusUpdated(StatusContainer *)), this,
            SIGNAL(statusUpdated(StatusContainer *)));

    emit statusUpdated(this);
}

void StatusContainerManager::setStatusModeChanged()
{
    cleanStatusContainers();
    if (m_statusConfigurationHolder->isSetStatusPerIdentity())
        addAllIdentities();
    else if (m_statusConfigurationHolder->isSetStatusPerAccount())
        addAllAccounts();
    else
        registerStatusContainer(m_allAccountsStatusContainer);
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
    if (StatusContainers.isEmpty())
        setDefaultStatusContainer(statusContainer);

    emit statusContainerAboutToBeRegistered(statusContainer);
    StatusContainers.append(statusContainer);
    emit statusContainerRegistered(statusContainer);
    StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);

    connect(statusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SIGNAL(statusUpdated(StatusContainer *)));
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
    emit statusContainerAboutToBeUnregistered(statusContainer);
    StatusContainers.removeAll(statusContainer);
    emit statusContainerUnregistered(statusContainer);
    StatusContainerAwareObject::notifyStatusContainerUnregistered(statusContainer);

    if (statusContainer == DefaultStatusContainer)
    {
        if (StatusContainers.isEmpty())
            setDefaultStatusContainer(0);
        else
            setDefaultStatusContainer(StatusContainers.at(0));
    }

    disconnect(statusContainer, 0, this, 0);
}

bool StatusContainerManager::allStatusOfType(StatusType type)
{
    if (StatusType::None == type)
        return false;

    foreach (StatusContainer *container, StatusContainers)
        if (container->status().type() != type)
            return false;
    return true;
}

QString StatusContainerManager::statusContainerName()
{
    return tr("All");
}

void StatusContainerManager::setStatus(Status status, StatusChangeSource source)
{
    foreach (StatusContainer *container, StatusContainers)
        container->setStatus(status, source);
}

Status StatusContainerManager::status()
{
    return DefaultStatusContainer ? DefaultStatusContainer->status() : Status();
}

bool StatusContainerManager::isStatusSettingInProgress()
{
    return DefaultStatusContainer ? DefaultStatusContainer->isStatusSettingInProgress() : false;
}

KaduIcon StatusContainerManager::statusIcon()
{
    return statusIcon(status());
}

KaduIcon StatusContainerManager::statusIcon(const Status &status)
{
    if (!DefaultStatusContainer)
        return m_statusTypeManager->statusIcon("common", Status{StatusType::Offline});

    return m_statusTypeManager->statusIcon("common", status);
}

QList<StatusType> StatusContainerManager::supportedStatusTypes()
{
    return DefaultStatusContainer ? DefaultStatusContainer->supportedStatusTypes() : QList<StatusType>();
}

int StatusContainerManager::maxDescriptionLength()
{
    return DefaultStatusContainer ? DefaultStatusContainer->maxDescriptionLength() : -1;
}

QString StatusContainerManager::statusNamePrefix()
{
    return tr("All") + ' ';
}

Status StatusContainerManager::loadStatus()
{
    return DefaultStatusContainer ? DefaultStatusContainer->loadStatus() : Status();
}

void StatusContainerManager::storeStatus(Status status)
{
    foreach (StatusContainer *statusContainer, StatusContainers)
        statusContainer->storeStatus(status);
}

QList<StatusContainer *> StatusContainerManager::subStatusContainers()
{
    return StatusContainers;
}

StatusContainer *StatusContainerManager::statusContainerForAccount(Account account) const
{
    if (m_statusConfigurationHolder->isSetStatusPerAccount())
        return account.statusContainer();
    if (m_statusConfigurationHolder->isSetStatusPerIdentity())
        return account.accountIdentity().statusContainer();
    return m_allAccountsStatusContainer;
}

#include "moc_status-container-manager.cpp"
