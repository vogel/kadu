/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "file-transfer-shared.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-error.h"
#include "file-transfer/file-transfer-status.h"
#include "misc/change-notifier.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"

FileTransferShared * FileTransferShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	auto result = loadFromStorage(fileTransferStoragePoint);
	result->loadStub();
	return result;
}

FileTransferShared * FileTransferShared::loadFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	auto result = new FileTransferShared();
	result->setStorage(fileTransferStoragePoint);
	return result;
}

FileTransferShared::FileTransferShared(const QUuid &uuid) :
		Shared{uuid},
		m_fileSize{0},
		m_transferredSize{0},
		m_transferType{FileTransferDirection::Incoming},
		m_transferStatus{FileTransferStatus::NotConnected},
		m_transferError{FileTransferError::NoError},
		m_handler{0}
{
	m_peer = new Contact();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

FileTransferShared::~FileTransferShared()
{
	ref.ref();

	delete m_peer;
}

StorableObject * FileTransferShared::storageParent()
{
	return FileTransferManager::instance();
}

QString FileTransferShared::storageNodeName()
{
	return QLatin1String("FileTransfer");
}

void FileTransferShared::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	*m_peer = ContactManager::instance()->byUuid(loadValue<QString>("Peer"));
	m_localFileName = loadValue<QString>("LocalFileName");
	m_remoteFileName = loadValue<QString>("RemoteFileName");
	m_transferType = ("Send" == loadValue<QString>("TransferType")) ? FileTransferDirection::Outgoing : FileTransferDirection::Incoming;
	m_fileSize = loadValue<qulonglong>("FileSize");
	m_transferredSize = loadValue<qulonglong>("TransferredSize");

	if ((m_fileSize == m_transferredSize) && m_fileSize != 0)
		m_transferStatus = FileTransferStatus::Finished;
}

void FileTransferShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Peer", m_peer->uuid().toString());
	storeValue("LocalFileName", m_localFileName);
	storeValue("RemoteFileName", m_remoteFileName);
	storeValue("TransferType", FileTransferDirection::Outgoing == m_transferType ? "Send" : "Receive");
	storeValue("FileSize", (qulonglong)m_fileSize);
	storeValue("TransferredSize", (qulonglong)m_transferredSize);
}

void FileTransferShared::setTransferStatus(FileTransferStatus transferStatus)
{
	ensureLoaded();

	if (m_transferStatus == transferStatus)
		return;

	m_transferStatus = transferStatus;
	emit statusChanged();
	changeNotifier().notify();
}

void FileTransferShared::setTransferError(FileTransferError transferError)
{
	ensureLoaded();

	if (m_transferStatus == FileTransferStatus::NotConnected && m_transferError == transferError)
		return;

	m_transferStatus = FileTransferStatus::NotConnected;
	m_transferError = transferError;
	emit statusChanged();
	changeNotifier().notify();
}

void FileTransferShared::setHandler(FileTransferHandler *handler)
{
	ensureLoaded();

	if (m_handler == handler)
		return;

	if (m_handler)
		disconnect(m_handler, 0, this, 0);
	m_handler = handler;
	if (m_handler)
		connect(m_handler, SIGNAL(destroyed()), this, SLOT(handlerDestroyed()));

	changeNotifier().notify();
}

void FileTransferShared::createHandler()
{
	if (m_handler)
		return;

	auto protocol = m_peer->contactAccount().protocolHandler();
	if (!protocol)
		return;

	auto service = protocol->fileTransferService();
	if (!service)
		return;

	m_handler = service->createFileTransferHandler(this);
}

void FileTransferShared::handlerDestroyed()
{
	m_handler = 0;
	changeNotifier().notify();
}

KaduShared_PropertyPtrDefCRW_M(FileTransferShared, Contact, peer, Peer)

#include "moc_file-transfer-shared.cpp"
