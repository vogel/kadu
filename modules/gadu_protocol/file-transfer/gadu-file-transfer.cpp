/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "dcc/dcc-manager.h"
#include "dcc/dcc-socket-notifiers.h"
#include "gadu-contact-account-data.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer.h"

GaduFileTransfer::GaduFileTransfer(Account account) :
		FileTransfer(account),
		SocketNotifiers(0), WaitingForSocketNotifiers(false)
{
}

GaduFileTransfer::GaduFileTransfer(Account account, Contact peer, FileTransfer::FileTransferType transferType) :
		FileTransfer(account, peer, transferType),
		SocketNotifiers(0), WaitingForSocketNotifiers(false)
{
}

GaduFileTransfer::~GaduFileTransfer()
{
	if (SocketNotifiers)
	{
		delete SocketNotifiers;
		SocketNotifiers = 0;
	}
}

void GaduFileTransfer::updateFileInfo()
{
	if (SocketNotifiers)
	{
		setFileSize(SocketNotifiers->fileSize());
		setTransferredSize(SocketNotifiers->transferredFileSize());
	}
	else
	{
		setFileSize(0);
		setTransferredSize(0);
	}

// 	emit statusChanged();
}

void GaduFileTransfer::setFileTransferNotifiers(DccSocketNotifiers *socketNotifiers)
{
	if (!socketNotifiers)
	{
		socketNotAvailable();
		return;
	}

	setRemoteFile(socketNotifiers->remoteFileName());
	setFileSize(socketNotifiers->fileSize());
	setTransferredSize(socketNotifiers->transferredFileSize());

	SocketNotifiers = socketNotifiers;
	connect(SocketNotifiers, SIGNAL(destroyed(QObject *)), this, SLOT(socketNotifiersDeleted()));

	SocketNotifiers->setGaduFileTransfer(this);
	WaitingForSocketNotifiers = false;
	setRemoteFile(socketNotifiers->remoteFileName());

	changeFileTransferStatus(FileTransfer::StatusTransfer);
}

void GaduFileTransfer::socketNotAvailable()
{
	WaitingForSocketNotifiers = false;

	changeFileTransferStatus(FileTransfer::StatusNotConnected);
}

void GaduFileTransfer::finished(bool ok)
{
	changeFileTransferStatus(ok
			? FileTransfer::StatusFinished
			: FileTransfer::StatusNotConnected);
}

void GaduFileTransfer::socketNotifiersDeleted()
{
	SocketNotifiers = 0;
}

void GaduFileTransfer::send()
{
	if (FileTransfer::TypeSend != transferType()) // maybe assert here?
		return;

	if (SocketNotifiers || WaitingForSocketNotifiers) // already sending/receiving
		return;

	setRemoteFile(QString::null);

	if (account().isNull() || localFileName().isEmpty())
	{
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
		return; // TODO: notify
	}

	GaduProtocol *gaduProtocol = dynamic_cast<GaduProtocol *>(account().protocolHandler());
	if (!gaduProtocol)
	{
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
		return;
	}

	GaduContactAccountData *gcad = gaduProtocol->gaduContactAccountData(contact());
	if (!gcad)
	{
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
		return;
	}

	// async call, will return in setFileTransferNotifiers
	changeFileTransferStatus(FileTransfer::StatusWaitingForConnection);
	WaitingForSocketNotifiers = true;
	gaduProtocol->dccManager()->attachSendFileTransferSocket(this);
}

void GaduFileTransfer::stop()
{
	if (SocketNotifiers)
	{
		delete SocketNotifiers;
		SocketNotifiers = 0;
		changeFileTransferStatus(FileTransfer::StatusNotConnected);
	}
}

void GaduFileTransfer::pause()
{
	stop();
}

void GaduFileTransfer::restore()
{
	if (FileTransfer::TypeSend == transferType())
		send();
}

bool GaduFileTransfer::accept(const QFile &file)
{
	FileTransfer::accept(file);
	return SocketNotifiers->acceptFileTransfer(file);
}

void GaduFileTransfer::reject()
{
	if (SocketNotifiers)
		SocketNotifiers->rejectFileTransfer();

	deleteLater();
}
