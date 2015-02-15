/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer.h"

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"

#include <QtCore/QFile>

KaduSharedBaseClassImpl(FileTransfer)

FileTransfer FileTransfer::null;

FileTransfer FileTransfer::create()
{
	return new FileTransferShared();
}

FileTransfer FileTransfer::loadStubFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	return FileTransferShared::loadStubFromStorage(fileTransferStoragePoint);
}

FileTransfer FileTransfer::loadFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint)
{
	return FileTransferShared::loadFromStorage(fileTransferStoragePoint);
}

FileTransfer::FileTransfer()
{
}

FileTransfer::FileTransfer(FileTransferShared *data) :
		SharedBase<FileTransferShared>{data}
{
}

FileTransfer::FileTransfer(QObject *data)
{
	auto shared = qobject_cast<FileTransferShared *>(data);
	if (shared)
		setData(shared);
}

FileTransfer::FileTransfer(const FileTransfer &copy) :
		SharedBase<FileTransferShared>{copy}
{
}

FileTransfer::~FileTransfer()
{
}

unsigned int FileTransfer::percent()
{
	if (fileSize() != 0)
		return static_cast<unsigned int>((100 * transferredSize()) / fileSize());
	else
		return 0;
}

KaduSharedBase_PropertyDefCRW(FileTransfer, Contact, peer, Peer, Contact::null)
KaduSharedBase_PropertyDefCRW(FileTransfer, QString, localFileName, LocalFileName, QString())
KaduSharedBase_PropertyDefCRW(FileTransfer, QString, remoteFileName, RemoteFileName, QString())
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, fileSize, FileSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, transferredSize, TransferredSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, QString, error, Error, QString{})
KaduSharedBase_PropertyDef(FileTransfer, FileTransferDirection, transferDirection, TransferDirection, FileTransferDirection::Outgoing)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferHandler *, handler, Handler, 0)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferStatus, transferStatus, TransferStatus, FileTransferStatus::NotConnected)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferType, transferType, TransferType, FileTransferType::Unknown)
