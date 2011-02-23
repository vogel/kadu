/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "dcc/dcc-socket-notifiers.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer-handler.h"

GaduFileTransferHandler::GaduFileTransferHandler(FileTransfer fileTransfer) :
		FileTransferHandler(fileTransfer),
		SocketNotifiers(0), WaitingForSocketNotifiers(false)
{
}

GaduFileTransferHandler::~GaduFileTransferHandler()
{
	if (SocketNotifiers)
	{
		delete SocketNotifiers;
		SocketNotifiers = 0;
	}
}

void GaduFileTransferHandler::updateFileInfo()
{
	transfer().blockUpdatedSignal();

	if (SocketNotifiers)
	{
		transfer().setFileSize(SocketNotifiers->fileSize());
		transfer().setTransferredSize(SocketNotifiers->transferredFileSize());
	}
	else
	{
		transfer().setFileSize(0);
		transfer().setTransferredSize(0);
	}

	transfer().unblockUpdatedSignal();
}

void GaduFileTransferHandler::setFileTransferNotifiers(DccSocketNotifiers *socketNotifiers)
{
	if (!socketNotifiers)
	{
		socketNotAvailable();
		return;
	}

	SocketNotifiers = socketNotifiers;
	if (SocketNotifiers)
	{
		transfer().setRemoteFileName(SocketNotifiers->remoteFileName());
		transfer().setFileSize(SocketNotifiers->fileSize());
		transfer().setTransferredSize(SocketNotifiers->transferredFileSize());
		transfer().setTransferStatus(StatusTransfer);

		connect(SocketNotifiers, SIGNAL(destroyed()), this, SLOT(socketNotifiersDeleted()));

		SocketNotifiers->setGaduFileTransferHandler(this);
	}

	WaitingForSocketNotifiers = false;
}

void GaduFileTransferHandler::socketNotAvailable()
{
	WaitingForSocketNotifiers = false;

	transfer().setTransferStatus(StatusNotConnected);
}

void GaduFileTransferHandler::finished(bool ok)
{
	transfer().setTransferStatus(ok
			? StatusFinished
			: StatusNotConnected);
}

void GaduFileTransferHandler::socketNotifiersDeleted()
{
	SocketNotifiers = 0;
}

void GaduFileTransferHandler::send()
{
	if (TypeSend != transfer().transferType()) // maybe assert here?
		return;

	if (SocketNotifiers || WaitingForSocketNotifiers) // already sending/receiving
		return;

	Contact contact = transfer().peer();
	Account account = contact.contactAccount();
	transfer().setRemoteFileName(QString());

	if (account.isNull() || transfer().localFileName().isEmpty())
	{
		transfer().setTransferStatus(StatusNotConnected);
		return; // TODO: notify
	}

	GaduProtocol *gaduProtocol = qobject_cast<GaduProtocol *>(account.protocolHandler());
	if (!gaduProtocol || !gaduProtocol->gaduContactDetails(contact))
	{
		transfer().setTransferStatus(StatusNotConnected);
		return;
	}

	// async call, will return in setFileTransferNotifiers
	transfer().setTransferStatus(StatusWaitingForConnection);
	WaitingForSocketNotifiers = true;

	if (gaduProtocol->fileTransferService())
		dynamic_cast<GaduFileTransferService *>(gaduProtocol->fileTransferService())->attachSendFileTransferSocket(this);
}

void GaduFileTransferHandler::stop()
{
	if (SocketNotifiers)
	{
		delete SocketNotifiers;
		SocketNotifiers = 0;
		transfer().setTransferStatus(StatusNotConnected);
	}
}

void GaduFileTransferHandler::pause()
{
	stop();
}

void GaduFileTransferHandler::restore()
{
	if (TypeSend == transfer().transferType())
		send();
}

bool GaduFileTransferHandler::accept(const QFile &file)
{
	transfer().accept(file);
	return SocketNotifiers->acceptFileTransfer(file);
}

void GaduFileTransferHandler::reject()
{
	if (SocketNotifiers)
		SocketNotifiers->rejectFileTransfer();

	deleteLater();
}
