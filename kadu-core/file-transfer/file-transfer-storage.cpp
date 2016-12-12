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

#include "file-transfer-storage.h"

#include "core/injected-factory.h"
#include "file-transfer/file-transfer-shared.h"
#include "file-transfer/file-transfer.h"

#include <QtWidgets/QApplication>

FileTransferStorage::FileTransferStorage(QObject *parent) :
		QObject{parent}
{
}

FileTransferStorage::~FileTransferStorage()
{
}

void FileTransferStorage::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

FileTransfer FileTransferStorage::create()
{
	return m_injectedFactory->makeInjected<FileTransferShared>();
}

FileTransfer FileTransferStorage::loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	auto result = m_injectedFactory->makeInjected<FileTransferShared>();
	result->setStorage(storagePoint);
	result->loadStub();

	return result;
}

FileTransfer FileTransferStorage::loadFromStorage(const std::shared_ptr<StoragePoint> &storagePoint)
{
	auto result = m_injectedFactory->makeInjected<FileTransferShared>();
	result->setStorage(storagePoint);

	return result;
}

#include "moc_file-transfer-storage.cpp"
