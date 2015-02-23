/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>

#include "file-transfer/outgoing-file-transfer-handler.h"

class JabberOutgoingFileTransferHandler : public OutgoingFileTransferHandler
{
	Q_OBJECT

	FileTransfer *JabberTransfer;
	// Jid PeerJid;

	bool InProgress;
	qlonglong BytesTransferred;
	QPointer<QIODevice> Source;

	void connectJabberTransfer();
	void disconnectJabberTransfer();

	FileTransferStatus errorToStatus(int error);
	void cleanup(FileTransferStatus status);

protected:
	virtual void updateFileInfo();

private slots:
	void fileTransferAccepted();
	void fileTransferConnected();
	void fileTransferBytesWritten(int);
	void fileTransferError(int);

public:
	explicit JabberOutgoingFileTransferHandler(FileTransfer fileTransfer);
	virtual ~JabberOutgoingFileTransferHandler();

	void setJTransfer(FileTransfer *jTransfer);

	virtual void send(QIODevice *source);
	virtual void stop();

signals:
	void statusChanged();

};
