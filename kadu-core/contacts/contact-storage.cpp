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

#include "contact-storage.h"

#include "contacts/contact-shared.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"

#include <QtWidgets/QApplication>

ContactStorage::ContactStorage(QObject *parent) : QObject{parent}
{
}

ContactStorage::~ContactStorage()
{
}

void ContactStorage::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

Contact ContactStorage::create()
{
    return m_injectedFactory->makeInjected<ContactShared>();
}

Contact ContactStorage::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<ContactShared>();
    result->setStorage(storagePoint);
    result->loadStub();

    return result;
}

Contact ContactStorage::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
    auto result = m_injectedFactory->makeInjected<ContactShared>();
    result->setStorage(storagePoint);

    return result;
}

#include "moc_contact-storage.cpp"
