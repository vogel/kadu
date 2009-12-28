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
#include "file-transfer/file-transfer.h"

#include "exports.h"

class FileTransferHandler;

class KADUAPI FileTransferService : public QObject
{
	Q_OBJECT

	friend class DccManager;

public:
	FileTransferService(QObject *parent = 0) : QObject(parent) {}

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer) = 0;

signals:
	void incomingFileTransfer(FileTransfer fileTransfer);

};

#endif // FILE_TRANSFER_SERVICE_H
