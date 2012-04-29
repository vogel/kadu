/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FILE_TRANSFER_SHARED_H
#define FILE_TRANSFER_SHARED_H

#include "file-transfer/file-transfer-enums.h"
#include "storage/shared.h"

class Contact;
class FileTransferHandler;

class KADUAPI FileTransferShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferShared)

	Contact *Peer;
	QString LocalFileName;
	QString RemoteFileName;

	unsigned long FileSize;
	unsigned long TransferredSize;

	FileTransferType TransferType;
	FileTransferStatus TransferStatus;
	FileTransferError TransferError;

	FileTransferHandler *Handler;

private slots:
	void handlerDestroyed();

protected:
	virtual void load();
	virtual void store();

public:
	static FileTransferShared * loadStubFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint);
	static FileTransferShared * loadFromStorage(const QSharedPointer<StoragePoint> &fileTransferStoragePoint);

	explicit FileTransferShared(const QUuid &uuid = QUuid());
	virtual ~FileTransferShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setTransferStatus(FileTransferStatus transferStatus);
	void setTransferError(FileTransferError transferError);
	void setHandler(FileTransferHandler *handler);
	void createHandler();

	KaduShared_PropertyDeclCRW(Contact, peer, Peer)
	KaduShared_Property(const QString &, localFileName, LocalFileName)
	KaduShared_Property(const QString &, remoteFileName, RemoteFileName)
	KaduShared_Property(unsigned long, fileSize, FileSize)
	KaduShared_Property(unsigned long, transferredSize, TransferredSize)
	KaduShared_Property(FileTransferType, transferType, TransferType)
	KaduShared_PropertyRead(FileTransferStatus, transferStatus, TransferStatus)
	KaduShared_PropertyRead(FileTransferError, transferError, TransferError)
	KaduShared_PropertyRead(FileTransferHandler *, handler, Handler)

signals:
	void statusChanged();

	void updated();

};

#endif // FILE_TRANSFER_SHARED_H
