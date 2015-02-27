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

#include "file-transfer/file-transfer-status.h"

#include "jabber-account-details.h"
#include "jabber-protocol.h"

#include "jabber-outgoing-file-transfer-handler.h"

JabberOutgoingFileTransferHandler::JabberOutgoingFileTransferHandler(::FileTransfer transfer) :
		OutgoingFileTransferHandler(transfer), JabberTransfer(0), InProgress(false), BytesTransferred(0)
{
}

JabberOutgoingFileTransferHandler::~JabberOutgoingFileTransferHandler()
{
	cleanup(transfer().transferStatus());
}

void JabberOutgoingFileTransferHandler::connectJabberTransfer()
{
	if (!JabberTransfer)
		return;

	// connect(JabberTransfer, SIGNAL(accepted()), this, SLOT(fileTransferAccepted()));
	// connect(JabberTransfer, SIGNAL(connected()), this, SLOT(fileTransferConnected()));
	// connect(JabberTransfer, SIGNAL(bytesWritten(int)), this, SLOT(fileTransferBytesWritten(int)));
	// connect(JabberTransfer, SIGNAL(error(int)), this, SLOT(fileTransferError(int)));
}

void JabberOutgoingFileTransferHandler::disconnectJabberTransfer()
{
// 	if (JabberTransfer)
// 		disconnect(JabberTransfer, 0, this, 0);
}

void JabberOutgoingFileTransferHandler::setJTransfer(FileTransfer *jTransfer)
{
	disconnectJabberTransfer();
	JabberTransfer = jTransfer;
	connectJabberTransfer();
}

void JabberOutgoingFileTransferHandler::cleanup(FileTransferStatus status)
{
	InProgress = false;

	transfer().setTransferStatus(status);

	if (JabberTransfer)
	{
		// JabberTransfer->deleteLater();
		JabberTransfer = nullptr;
	}

	if (Source)
	{
		Source->close();
		Source->deleteLater();
	}

	deleteLater();
}

void JabberOutgoingFileTransferHandler::updateFileInfo()
{
	if (JabberTransfer)
		transfer().setTransferredSize(BytesTransferred);
	else
		transfer().setTransferredSize(0);

	emit statusChanged();
}

void JabberOutgoingFileTransferHandler::send(QIODevice *source)
{
	if (InProgress) // already sending/receiving
		return;

	Source = source;

	Account account = transfer().peer().contactAccount();
	if (account.isNull())
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		deleteLater();
		return; // TODO: notify
	}

	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(account.protocolHandler());
	if (!jabberProtocol)
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		deleteLater();
		return;
	}

	if (!jabberProtocol->jabberContactDetails(transfer().peer()))
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		deleteLater();
		return;
	}

	QString jid = transfer().peer().id();
	// sendFile needs jid with resource so take best from ResourcePool
	// PeerJid = Jid(jid).withResource(jabberProtocol->resourcePool()->bestResource(jid).name());

	if (!JabberTransfer)
	{
		// JabberTransfer = jabberProtocol->xmppClient()->fileTransferManager()->createTransfer();
		connectJabberTransfer();
	}

	// JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account.details());
	// Jid proxy;
	// if (0 != jabberAccountDetails)
		// proxy = jabberAccountDetails->dataTransferProxy();

// 	if (proxy.isValid())
// 		JabberTransfer->setProxy(proxy);

	transfer().setTransferStatus(FileTransferStatus::WaitingForAccept);
	InProgress = true;

// 	JabberTransfer->sendFile(PeerJid, transfer().remoteFileName(), transfer().fileSize(), QString(), FTThumbnail());
}

void JabberOutgoingFileTransferHandler::stop()
{
// 	if (JabberTransfer)
// 		JabberTransfer->close();

	cleanup(FileTransferStatus::NotConnected);
}

void JabberOutgoingFileTransferHandler::fileTransferAccepted()
{
	transfer().setTransferStatus(FileTransferStatus::WaitingForConnection);
}

void JabberOutgoingFileTransferHandler::fileTransferConnected()
{
	if (!Source->isOpen()) // ?? assert
	{
		cleanup(FileTransferStatus::NotConnected);
		return;
	}

	// BytesTransferred = JabberTransfer->offset();
	if (0 != BytesTransferred && !Source->seek(BytesTransferred))
	{
		cleanup(FileTransferStatus::NotConnected);
		return;
	}

	transfer().setTransferredSize(BytesTransferred);
	fileTransferBytesWritten(0);

	transfer().setTransferStatus(FileTransferStatus::Transfer);
}

void JabberOutgoingFileTransferHandler::fileTransferBytesWritten(int written)
{
	BytesTransferred += written;
	updateFileInfo();

	if (BytesTransferred == (qlonglong)(transfer().fileSize()))
	{
		cleanup(FileTransferStatus::Finished);
		return;
	}

// 	if (!JabberTransfer->bsConnection())
	// {
		// cleanup(FileTransferStatus::NotConnected);
		// return;
	// }

	// int dataSize = JabberTransfer->dataSizeNeeded();
	// QByteArray data(dataSize, (char)0);

	// int sizeRead = Source->read(data.data(), data.size());
	// if (sizeRead < 0)
	// {
// 		cleanup(FileTransferStatus::NotConnected);
		// return;
	// }

	// if (sizeRead < data.size())
		// data.resize(sizeRead);

	// JabberTransfer->writeFileData(data);
}

FileTransferStatus JabberOutgoingFileTransferHandler::errorToStatus(int error)
{
	switch (error)
	{
// 		case FileTransfer::ErrReject:
	// 		return FileTransferStatus::Rejected;
		// 	break;
		// case FileTransfer::ErrNeg:
		// case FileTransfer::ErrConnect:
		// case FileTransfer::ErrStream:
		default:
			return FileTransferStatus::NotConnected;
			break;
	}
}

void JabberOutgoingFileTransferHandler::fileTransferError(int error)
{
	cleanup(errorToStatus(error));
}

#include "moc_jabber-outgoing-file-transfer-handler.cpp"
