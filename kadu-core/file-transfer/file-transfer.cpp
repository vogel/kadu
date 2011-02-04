/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QFile>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/xml-configuration-file.h"
#include "file-transfer/file-transfer-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/file-transfer-service.h"
#include "storage/storage-point.h"

#include "file-transfer.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"

KaduSharedBaseClassImpl(FileTransfer)

FileTransfer FileTransfer::null;

FileTransfer FileTransfer::create()
{
	return new FileTransferShared();
}

FileTransfer FileTransfer::loadStubFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint)
{
	return FileTransferShared::loadStubFromStorage(fileTransferStoragePoint);
}

FileTransfer FileTransfer::loadFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint)
{
	return FileTransferShared::loadFromStorage(fileTransferStoragePoint);
}

FileTransfer::FileTransfer()
{
}

FileTransfer::FileTransfer(FileTransferShared *data) :
		SharedBase<FileTransferShared>(data)
{
}

FileTransfer::FileTransfer(QObject *data)
{
	FileTransferShared *shared = qobject_cast<FileTransferShared *>(data);
	if (shared)
		setData(shared);
}

FileTransfer::FileTransfer(const FileTransfer &copy) :
		SharedBase<FileTransferShared>(copy)
{
}

FileTransfer::~FileTransfer()
{
}

void FileTransfer::createHandler()
{
	if (!isNull())
		data()->createHandler();
}

unsigned int FileTransfer::percent()
{
	if (fileSize() != 0)
		return (100 * transferredSize()) / fileSize();
	else
		return 0;
}

bool FileTransfer::accept(const QFile &file)
{
	setLocalFileName(file.fileName());
	// XXX
	return true;
}

KaduSharedBase_PropertyDef(FileTransfer, Contact, peer, Peer, Contact::null)
KaduSharedBase_PropertyDef(FileTransfer, QString, localFileName, LocalFileName, QString())
KaduSharedBase_PropertyDef(FileTransfer, QString, remoteFileName, RemoteFileName, QString())
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, fileSize, FileSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, unsigned long, transferredSize, TransferredSize, 0)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferType, transferType, TransferType, TypeSend)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferStatus, transferStatus, TransferStatus, StatusNotConnected)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferError, transferError, TransferError, ErrorOk)
KaduSharedBase_PropertyDef(FileTransfer, FileTransferHandler *, handler, Handler, 0)
