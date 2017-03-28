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

#include "account-storage.h"

#include "accounts/account-shared.h"
#include "accounts/account.h"
#include "core/injected-factory.h"

AccountStorage::AccountStorage(QObject *parent) : QObject{parent}
{
}

AccountStorage::~AccountStorage()
{
}

void AccountStorage::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

Account AccountStorage::create(const QString &protocolName)
{
    return m_injectedFactory->makeInjected<AccountShared>(protocolName);
}

Account AccountStorage::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<AccountShared>();
    result->setStorage(storagePoint);
    result->loadStub();

    return result;
}

Account AccountStorage::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<AccountShared>();
    result->setStorage(storagePoint);

    return result;
}

#include "moc_account-storage.cpp"
