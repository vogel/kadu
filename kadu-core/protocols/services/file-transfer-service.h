/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_SERVICE_H
#define FILE_TRANSFER_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy.h"

#include "exports.h"

class FileTransfer;
class StoragePoint;

class KADUAPI FileTransferService : public QObject
{
	Q_OBJECT

	friend class DccManager;

public:
	FileTransferService(QObject *parent = 0) : QObject(parent) {}

	virtual FileTransfer * loadFileTransferFromStorage(StoragePoint *storage) = 0;
	virtual FileTransfer * createOutgoingFileTransfer(Buddy contact) = 0;

signals:
	void incomingFileTransfer(FileTransfer *fileTransfer);

};

#endif // FILE_TRANSFER_SERVICE_H
