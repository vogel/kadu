/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_MANAGER_H
#define FILE_TRANSFER_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/accounts-aware-object.h"
#include "configuration/storable-object.h"

#include "exports.h"

class FileTransfer;
class XmlConfigFile;

class KADUAPI FileTransferManager : public QObject, public AccountsAwareObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferManager)

	static FileTransferManager *Instance;

	FileTransferManager();
	virtual ~FileTransferManager();

	QList<FileTransfer *> FileTransfers;

	void loadConfigurationForAccount(Account *account);
	void storeConfigurationForAccount(Account *account);

private slots:
	void incomingFileTransfer(FileTransfer *fileTransfer);

protected:
	virtual StoragePoint * createStoragePoint();

	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);

public:
	static FileTransferManager * instance();

	QList<FileTransfer *> fileTransfer() { return FileTransfers; }
	void addFileTransfer(FileTransfer *fileTransfer);
	void removeFileTransfer(FileTransfer *fileTransfer);

	void cleanUp();

signals:
	void fileTransferAboutToBeAdded(FileTransfer *contact);
	void fileTransferAdded(FileTransfer *contact);
	void fileTransferAboutToBeRemoved(FileTransfer *contact);
	void fileTransferRemoved(FileTransfer *contact);

	void incomingFileTransferNeedAccept(FileTransfer *fileTransfer);

};

#endif // FILE_TRANSFER_MANAGER_H
