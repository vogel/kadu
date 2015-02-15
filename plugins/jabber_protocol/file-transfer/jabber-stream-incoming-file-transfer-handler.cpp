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

#include <QtCore/QFileInfo>

#include <xmpp/xmpp-im/xmpp_bytestream.h>
#include <filetransfer.h>

#include "file-transfer/file-transfer-status.h"

#include "resource/jabber-resource-pool.h"
#include "jabber-protocol.h"

#include "jabber-stream-incoming-file-transfer-handler.h"

JabberStreamIncomingFileTransferHandler::JabberStreamIncomingFileTransferHandler(::FileTransfer transfer) :
		StreamIncomingFileTransferHandler(transfer), JabberTransfer(0), InProgress(false), BytesTransferred(0)
{
}

JabberStreamIncomingFileTransferHandler::~JabberStreamIncomingFileTransferHandler()
{
	cleanup(transfer().transferStatus());
}

void JabberStreamIncomingFileTransferHandler::connectJabberTransfer()
{
	if (!JabberTransfer)
		return;

	connect(JabberTransfer, SIGNAL(accepted()), this, SLOT(fileTransferAccepted()));
	connect(JabberTransfer, SIGNAL(connected()), this, SLOT(fileTransferConnected()));
	connect(JabberTransfer, SIGNAL(readyRead(const QByteArray &)), this, SLOT(fileTransferReadyRead(const QByteArray &)));
	connect(JabberTransfer, SIGNAL(error(int)), this, SLOT(fileTransferError(int)));
}

void JabberStreamIncomingFileTransferHandler::disconnectJabberTransfer()
{
	if (JabberTransfer)
		disconnect(JabberTransfer, 0, this, 0);
}

void JabberStreamIncomingFileTransferHandler::setJTransfer(XMPP::FileTransfer *jTransfer)
{
	disconnectJabberTransfer();
	JabberTransfer = jTransfer;
	connectJabberTransfer();
}

void JabberStreamIncomingFileTransferHandler::cleanup(FileTransferStatus status)
{
	InProgress = false;

	transfer().setTransferStatus(status);

	if (JabberTransfer)
	{
		JabberTransfer->deleteLater();
		JabberTransfer = nullptr;
	}

	if (Destination)
	{
		Destination->close();
		Destination->deleteLater();
	}

	deleteLater();
}

void JabberStreamIncomingFileTransferHandler::updateFileInfo()
{
	if (JabberTransfer)
		transfer().setTransferredSize(BytesTransferred);
	else
		transfer().setTransferredSize(0);

	emit statusChanged();
}

void JabberStreamIncomingFileTransferHandler::accept(QIODevice *destination)
{
	Destination = destination;
	BytesTransferred = 0;

	transfer().setTransferStatus(FileTransferStatus::Transfer);
	transfer().setTransferredSize(BytesTransferred);
	transfer().setFileSize(JabberTransfer->fileSize());

	JabberTransfer->accept(BytesTransferred);
}

void JabberStreamIncomingFileTransferHandler::reject()
{
	if (JabberTransfer)
		JabberTransfer->close();

	deleteLater();
}

void JabberStreamIncomingFileTransferHandler::fileTransferAccepted()
{
	transfer().setTransferStatus(FileTransferStatus::WaitingForConnection);
}

void JabberStreamIncomingFileTransferHandler::fileTransferConnected()
{
	transfer().setTransferStatus(FileTransferStatus::Transfer);
}

void JabberStreamIncomingFileTransferHandler::fileTransferReadyRead(const QByteArray &a)
{
	if (Destination)
		Destination->write(a);

	BytesTransferred += a.size();
	updateFileInfo();

	if (BytesTransferred == JabberTransfer->fileSize())
		cleanup(FileTransferStatus::Finished);
}

FileTransferStatus JabberStreamIncomingFileTransferHandler::errorToStatus(int error)
{
	switch (error)
	{
		case XMPP::FileTransfer::ErrReject:
			return FileTransferStatus::Rejected;
			break;
		case XMPP::FileTransfer::ErrNeg:
		case XMPP::FileTransfer::ErrConnect:
		case XMPP::FileTransfer::ErrStream:
		default:
			return FileTransferStatus::NotConnected;
			break;
	}
}

void JabberStreamIncomingFileTransferHandler::fileTransferError(int error)
{
	cleanup(errorToStatus(error));
}

#include "moc_jabber-stream-incoming-file-transfer-handler.cpp"
