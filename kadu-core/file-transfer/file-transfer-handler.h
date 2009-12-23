/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_HANDLER_H
#define FILE_TRANSFER_HANDLER_H

#include <QtCore/QObject>

#include "file-transfer/file-transfer.h"

class FileTransferHandler : public QObject
{
	Q_OBJECT;

	FileTransfer Transfer;

public:
	FileTransferHandler(FileTransfer transfer) { Transfer = transfer; }
	virtual ~FileTransferHandler() {}

	FileTransfer transfer() { return Transfer; }
	void setTransfer(FileTransfer transfer) { Transfer = transfer; }

	virtual void send() = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void restore() = 0;

	virtual bool accept(const QFile &file) = 0;
	virtual void reject() = 0;

};

#endif // FILE_TRANSFER_HANDLER_H
