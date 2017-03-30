/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"
#include "identities/identities-aware-object.h"
#include "identities/identity-storage.h"

#include "identity-manager.h"

IdentityManager::IdentityManager(QObject *parent) : Manager<Identity>{parent}
{
}

IdentityManager::~IdentityManager()
{
}

void IdentityManager::setConfigurationManager(ConfigurationManager *configurationManager)
{
    m_configurationManager = configurationManager;
}

void IdentityManager::setIdentityStorage(IdentityStorage *identityStorage)
{
    m_identityStorage = identityStorage;
}

void IdentityManager::init()
{
    m_configurationManager->registerStorableObject(this);
}

void IdentityManager::done()
{
    m_configurationManager->unregisterStorableObject(this);
}

Identity IdentityManager::byName(const QString &name, bool create)
{
    QMutexLocker locker(&mutex());

    if (name.isEmpty())
        return Identity::null;

    for (auto const &identity : items())
    {
        if (name == identity.name())
            return identity;
    }

    if (!create)
        return Identity::null;

    auto newIdentity = m_identityStorage->create();
    newIdentity.setName(name);
    addItem(newIdentity);

    return newIdentity;
}

Identity IdentityManager::identityForAcccount(Account account)
{
    QMutexLocker locker(&mutex());

    for (auto const &identity : items())
        if (identity.hasAccount(account))
            return identity;

    return Identity::null;
}

void IdentityManager::removeUnused()
{
    QList<Identity> unused;

    for (auto const &identity : items())
        if (identity.isEmpty() && !identity.isPermanent())
            unused.append(identity);

    for (auto const &identity : unused)
        removeItem(identity);
}

void IdentityManager::itemAboutToBeAdded(Identity item)
{
    emit identityAboutToBeAdded(item);
}

void IdentityManager::itemAdded(Identity item)
{
    IdentitiesAwareObject::notifyIdentityAdded(item);

    emit identityAdded(item);
}

void IdentityManager::itemAboutToBeRemoved(Identity item)
{
    IdentitiesAwareObject::notifyIdentityRemoved(item);

    emit identityAboutToBeRemoved(item);
}

void IdentityManager::itemRemoved(Identity item)
{
    emit identityRemoved(item);
}

void IdentityManager::addDefaultIdentities()
{
    QMutexLocker locker(&mutex());

    auto friendsIdentity = m_identityStorage->create();
    friendsIdentity.data()->setState(StateNew);
    friendsIdentity.setPermanent(true);
    friendsIdentity.setName(tr("Friends"));
    addItem(friendsIdentity);

    auto workIdentity = m_identityStorage->create();
    workIdentity.data()->setState(StateNew);
    workIdentity.setPermanent(true);
    workIdentity.setName(tr("Work"));
    addItem(workIdentity);

    auto schoolIdentity = m_identityStorage->create();
    schoolIdentity.data()->setState(StateNew);
    schoolIdentity.setPermanent(true);
    schoolIdentity.setName(tr("School"));
    addItem(schoolIdentity);
}

void IdentityManager::load()
{
    QMutexLocker locker(&mutex());

    Manager<Identity>::load();

    if (items().isEmpty())
        addDefaultIdentities();
}

Identity IdentityManager::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    return m_identityStorage->loadStubFromStorage(storagePoint);
}

#include "moc_identity-manager.cpp"
