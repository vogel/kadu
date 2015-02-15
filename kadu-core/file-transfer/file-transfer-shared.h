/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "storage/shared.h"

class Contact;
class FileTransferHandler;

enum class FileTransferDirection;
enum class FileTransferStatus;
enum class FileTransferType;

class KADUAPI FileTransferShared : public QObject, public Shared
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferShared)

public:
	static FileTransferShared * loadStubFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint);
	static FileTransferShared * loadFromStorage(const std::shared_ptr<StoragePoint> &fileTransferStoragePoint);

	explicit FileTransferShared(const QUuid &uuid = QUuid{});
	virtual ~FileTransferShared();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();

	void setError(QString error);
	void setTransferStatus(FileTransferStatus transferStatus);
	void setHandler(FileTransferHandler *handler);

	KaduShared_PropertyDeclCRW(Contact, peer, Peer)
	KaduShared_Property_M(const QString &, localFileName, LocalFileName)
	KaduShared_Property_M(const QString &, remoteFileName, RemoteFileName)
	KaduShared_Property_M(unsigned long, fileSize, FileSize)
	KaduShared_Property_M(unsigned long, transferredSize, TransferredSize)
	KaduShared_Property_M(FileTransferDirection, transferDirection, TransferDirection)
	KaduShared_Property_M(FileTransferType, transferType, TransferType)
	KaduShared_PropertyRead_M(QString, error)
	KaduShared_PropertyRead_M(FileTransferStatus, transferStatus)
	KaduShared_PropertyRead_M(FileTransferHandler *, handler)

signals:
	void statusChanged();

	void updated();

protected:
	virtual void load();
	virtual void store();

private:
	Contact *m_peer;
	QString m_localFileName;
	QString m_remoteFileName;

	unsigned long m_fileSize;
	unsigned long m_transferredSize;

	QString m_error;
	FileTransferDirection m_transferDirection;
	FileTransferStatus m_transferStatus;
	FileTransferType m_transferType;

	FileTransferHandler *m_handler;

private slots:
	void handlerDestroyed();

};
