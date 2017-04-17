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

#include "group-storage.h"
#include "group-storage.moc"

#include "buddies/group-shared.h"
#include "buddies/group.h"
#include "core/injected-factory.h"

#include <QtWidgets/QApplication>

GroupStorage::GroupStorage(QObject *parent) : QObject{parent}
{
}

GroupStorage::~GroupStorage()
{
}

void GroupStorage::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

Group GroupStorage::create()
{
    return m_injectedFactory->makeInjected<GroupShared>();
}

Group GroupStorage::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<GroupShared>();
    result->setStorage(storagePoint);
    result->loadStub();

    return result;
}

Group GroupStorage::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<GroupShared>();
    result->setStorage(storagePoint);

    return result;
}
