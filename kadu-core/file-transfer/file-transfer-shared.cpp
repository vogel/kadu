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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "misc/change-notifier.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"

#include "file-transfer-shared.h"

FileTransferShared * FileTransferShared::loadStubFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	FileTransferShared *result = loadFromStorage(fileTransferStoragePoint);
	result->loadStub();
	return result;
}

FileTransferShared * FileTransferShared::loadFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	FileTransferShared *result = new FileTransferShared();
	result->setStorage(fileTransferStoragePoint);
	return result;
}

FileTransferShared::FileTransferShared(const QUuid &uuid) :
		Shared(uuid),
		FileSize(0), TransferredSize(0),
		TransferType(TypeReceive), TransferStatus(StatusNotConnected),
		TransferError(ErrorOk), Handler(0)
{
	Peer = new Contact();

	connect(&changeNotifier(), SIGNAL(changed()), this, SIGNAL(updated()));
}

FileTransferShared::~FileTransferShared()
{
	ref.ref();

	delete Peer;
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

	*Peer = ContactManager::instance()->byUuid(loadValue<QString>("Peer"));
	LocalFileName = loadValue<QString>("LocalFileName");
	RemoteFileName = loadValue<QString>("RemoteFileName");
	TransferType = ("Send" == loadValue<QString>("TransferType")) ? TypeSend : TypeReceive;
	FileSize = loadValue<qulonglong>("FileSize");
	TransferredSize = loadValue<qulonglong>("TransferredSize");

	if (FileSize == TransferredSize)
		TransferStatus = StatusFinished;
}

void FileTransferShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	storeValue("Peer", Peer->uuid().toString());
	storeValue("LocalFileName", LocalFileName);
	storeValue("RemoteFileName", RemoteFileName);
	storeValue("TransferType", TypeSend == TransferType ? "Send" : "Receive");
	storeValue("FileSize", (qulonglong)FileSize);
	storeValue("TransferredSize", (qulonglong)TransferredSize);
}

void FileTransferShared::setTransferStatus(FileTransferStatus transferStatus)
{
	ensureLoaded();

	if (TransferStatus == transferStatus)
		return;

	TransferStatus = transferStatus;
	emit statusChanged();
	changeNotifier().notify();
}

void FileTransferShared::setTransferError(FileTransferError transferError)
{
	ensureLoaded();

	if (TransferStatus == StatusNotConnected && TransferError == transferError)
		return;

	TransferStatus = StatusNotConnected;
	TransferError = transferError;
	emit statusChanged();
	changeNotifier().notify();
}

void FileTransferShared::setHandler(FileTransferHandler *handler)
{
	ensureLoaded();

	if (Handler == handler)
		return;

	if (Handler)
		disconnect(Handler, 0, this, 0);
	Handler = handler;
	if (Handler)
		connect(Handler, SIGNAL(destroyed()), this, SLOT(handlerDestroyed()));

	changeNotifier().notify();
}

void FileTransferShared::createHandler()
{
	if (Handler)
		return;

	Protocol *protocol = Peer->contactAccount().protocolHandler();
	if (!protocol)
		return;

	FileTransferService *service = protocol->fileTransferService();
	if (!service)
		return;

	Handler = service->createFileTransferHandler(this);
}

void FileTransferShared::handlerDestroyed()
{
	Handler = 0;
	changeNotifier().notify();
}

KaduShared_PropertyPtrDefCRW(FileTransferShared, Contact, peer, Peer)

#include "moc_file-transfer-shared.cpp"
