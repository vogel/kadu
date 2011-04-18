/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef FILE_TRANSFER_HANDLER_H
#define FILE_TRANSFER_HANDLER_H

#include <QtCore/QObject>

#include "file-transfer/file-transfer.h"
#include "exports.h"

class KADUAPI FileTransferHandler : public QObject
{
	Q_OBJECT

	FileTransfer Transfer;

public:
	FileTransferHandler(FileTransfer transfer) { Transfer = transfer; }
	virtual ~FileTransferHandler() {}

	FileTransfer transfer() { return Transfer; }
	void setTransfer(FileTransfer transfer) { Transfer = transfer; }

	virtual void send() = 0;
	virtual void stop() = 0;

	virtual bool accept(const QString &fileName, bool resumeTransfer) = 0;
	virtual void reject() = 0;

};

#endif // FILE_TRANSFER_HANDLER_H
