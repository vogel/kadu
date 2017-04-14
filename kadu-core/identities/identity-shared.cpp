/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "icons/kadu-icon.h"
#include "identities/identity-manager.h"
#include "identities/identity-status-container.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "status/status-type.h"

#include "identity-shared.h"
#include "identity-shared.moc"

IdentityShared::IdentityShared(const QUuid &uuid, QObject *parent) : Shared(uuid, parent), Permanent(false)
{
}

IdentityShared::~IdentityShared()
{
    ref.ref();
}

void IdentityShared::setIdentityManager(IdentityManager *identityManager)
{
    m_identityManager = identityManager;
}

void IdentityShared::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void IdentityShared::init()
{
    m_identityStatusContainer = m_injectedFactory->makeNotOwned<IdentityStatusContainer>(this);
    setState(StateNotLoaded);
}

StorableObject *IdentityShared::storageParent()
{
    return m_identityManager;
}

QString IdentityShared::storageNodeName()
{
    return QStringLiteral("Identity");
}

void IdentityShared::load()
{
    if (!isValidStorage())
        return;

    Shared::load();

    Permanent = loadValue<bool>("Permanent");
    Name = loadValue<QString>("Name");
}

void IdentityShared::store()
{
    if (!isValidStorage())
        return;

    Shared::store();

    storeValue("Permanent", Permanent);
    storeValue("Name", Name);
}

bool IdentityShared::shouldStore()
{
    ensureLoaded();

    return (UuidStorableObject::shouldStore() && !Name.isEmpty()) || customProperties()->shouldStore();
}

void IdentityShared::aboutToBeRemoved()
{
    Accounts.clear();
}

QList<Account> IdentityShared::accounts()
{
    ensureLoaded();

    return Accounts;
}

void IdentityShared::addAccount(const Account &account)
{
    if (!account)
        return;

    ensureLoaded();

    Accounts.append(account);
    m_identityStatusContainer->addAccount(account);
}

void IdentityShared::removeAccount(const Account &account)
{
    if (!account)
        return;

    ensureLoaded();

    if (Accounts.removeAll(account) > 0)
        m_identityStatusContainer->removeAccount(account);
}

bool IdentityShared::hasAccount(const Account &account)
{
    ensureLoaded();

    return account && Accounts.contains(account);
}

bool IdentityShared::hasAnyLoadedAccount()
{
    ensureLoaded();
    return std::any_of(std::begin(Accounts), std::end(Accounts), [](const Account &x) { return x.protocolHandler(); });
}

bool IdentityShared::isEmpty()
{
    ensureLoaded();

    return Accounts.isEmpty();
}

StatusContainer *IdentityShared::statusContainer() const
{
    return m_identityStatusContainer.get();
}
