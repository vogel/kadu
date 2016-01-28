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

#include "network-proxy-storage.h"

#include "core/injected-factory.h"
#include "network/proxy/network-proxy-shared.h"
#include "network/proxy/network-proxy.h"

#include <QtWidgets/QApplication>

NetworkProxyStorage::NetworkProxyStorage(QObject *parent) :
		QObject{parent}
{
}

NetworkProxyStorage::~NetworkProxyStorage()
{
}

void NetworkProxyStorage::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

NetworkProxy NetworkProxyStorage::create()
{
	return m_injectedFactory->makeInjected<NetworkProxyShared>();
}

NetworkProxy NetworkProxyStorage::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	auto result = m_injectedFactory->makeInjected<NetworkProxyShared>();
	result->setStorage(storagePoint);
	result->loadStub();

	return result;
}

NetworkProxy NetworkProxyStorage::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	auto result = m_injectedFactory->makeInjected<NetworkProxyShared>();
	result->setStorage(storagePoint);

	return result;
}

#include "moc_network-proxy-storage.cpp"
