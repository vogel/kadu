/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"
#include "dcc.h"
#include "dcc_socket.h"
#include "debug.h"
#include "file_transfer.h"
#include "file_transfer_manager.h"
#include "message_box.h"
#include "protocol.h"

#include "file_transfer.h"

uint32_t gg_fix32(uint32_t);

FileTransfer::FileTransfer(FileTransferManager *listener, DccVersion version,
		FileTransferType type, const UinType &contact, const QString &fileName) :
	QObject(0, 0), mainListener(listener), listeners(), Socket(0), Version(version), Type(type), Status(StatusFrozen),
	Contact(contact), FileName(fileName), GaduFileName(), connectionTimeoutTimer(0),
	updateFileInfoTimer(0), FileSize(0), TransferedSize(0), PrevTransferedSize(0),
	Speed(0)
{
	kdebugf();

	if (mainListener)
	{
		connectSignals(mainListener, false);

		connect(this, SIGNAL(fileTransferFinished(FileTransfer *)),
			mainListener, SLOT(fileTransferFinishedSlot(FileTransfer *)));
	}

	emit newFileTransfer(this);
	emit fileTransferStatusChanged(this);

	kdebugf2();
}

FileTransfer::~FileTransfer()
{
	kdebugf();

	Status = StatusFinished;
	Speed = 0;

	emit fileTransferStatusChanged(this);
	emit fileTransferDestroying(this);

	FOREACH(i, listeners)
		disconnectSignals((*i).first, (*i).second);

	if (mainListener)
		disconnectSignals(mainListener, false);

	file_transfer_manager->removeTransfer(this);

	if (Socket)
	{
		kdebugmf(KDEBUG_WARNING, "DCC transfer has not finished yet!\n");
		Socket->stop();
		Socket = 0;
	}

	cancelTimeout();
	stopUpdateFileInfo();

	kdebugf2();
}

void FileTransfer::connectSignals(QObject *object, bool listenerHasSlots)
{
	kdebugf();

	if (listenerHasSlots)
	{
		connect(this, SIGNAL(newFileTransfer(FileTransfer *)), object, SLOT(newFileTransfer(FileTransfer *)));
		connect(this, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)),
			object, SLOT(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)));
		connect(this, SIGNAL(fileTransferStatusChanged(FileTransfer *)),
			object, SLOT(fileTransferStatusChanged(FileTransfer *)));
		connect(this, SIGNAL(fileTransferFinished(FileTransfer *)),
			object, SLOT(fileTransferFinished(FileTransfer *)));
		connect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			object, SLOT(fileTransferDestroying(FileTransfer *)));
	}
	else
	{
		connect(this, SIGNAL(newFileTransfer(FileTransfer *)), object, SIGNAL(newFileTransfer(FileTransfer *)));
		connect(this, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)),
			object, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)));
		connect(this, SIGNAL(fileTransferStatusChanged(FileTransfer *)),
			object, SIGNAL(fileTransferStatusChanged(FileTransfer *)));
		connect(this, SIGNAL(fileTransferFinished(FileTransfer *)),
			object, SIGNAL(fileTransferFinished(FileTransfer *)));
		connect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			object, SIGNAL(fileTransferDestroying(FileTransfer *)));
	}
}

void FileTransfer::disconnectSignals(QObject *object, bool listenerHasSlots)
{
	kdebugf();

	if (listenerHasSlots)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)), object, SLOT(newFileTransfer(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)),
			object, SLOT(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)));
		disconnect(this, SIGNAL(fileTransferStatusChanged(FileTransfer *)),
			object, SLOT(fileTransferStatusChanged(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferFinished(FileTransfer *)),
			object, SLOT(fileTransferFinished(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			object, SLOT(fileTransferDestroying(FileTransfer *)));
	}
	else
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)), object, SIGNAL(newFileTransfer(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)),
			object, SIGNAL(fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)));
		disconnect(this, SIGNAL(fileTransferStatusChanged(FileTransfer *)),
			object, SIGNAL(fileTransferStatusChanged(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferFinished(FileTransfer *)),
			object, SIGNAL(fileTransferFinished(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			object, SIGNAL(fileTransferDestroying(FileTransfer *)));
	}
}

void FileTransfer::addListener(QObject *listener, bool listenerHasSlots)
{
	kdebugf();

	listeners.push_back(qMakePair(listener, listenerHasSlots));
	connectSignals(listener, listenerHasSlots);
}

void FileTransfer::removeListener(QObject *listener, bool listenerHasSlots)
{
	kdebugf();

	disconnectSignals(listener, listenerHasSlots);
	listeners.remove(qMakePair(listener, listenerHasSlots));
}

FileTransfer::FileTransferType FileTransfer::type()
{
	kdebugf();

	return Type;
}

FileTransfer::FileTransferStatus FileTransfer::status()
{
	kdebugf();

	return Status;
}

UinType FileTransfer::contact()
{
	kdebugf();

	return Contact;
}

QString FileTransfer::fileName()
{
	kdebugf();

	return FileName;
}

int FileTransfer::percent()
{
	kdebugf();

	if (FileSize != 0)
		return (100 * TransferedSize) / FileSize;
	else
		return 0;
}

long int FileTransfer::speed()
{
	kdebugf();

	return Speed;
}

long long int FileTransfer::fileSize()
{
	kdebugf();

	return FileSize;
}

long long int FileTransfer::transferedSize()
{
	kdebugf();

	return TransferedSize;
}

QDomElement FileTransfer::toDomElement(const QDomElement &root)
{
	QDomElement dom = xml_config_file->createElement(root, "FileTransfer");
	dom.setAttribute("Type", static_cast<int>(Type));
	dom.setAttribute("Contact", static_cast<int>(Contact));
	dom.setAttribute("FileName", FileName);
	dom.setAttribute("GaduFileName", GaduFileName);
	dom.setAttribute("FileSize", QString::number(FileSize));
	dom.setAttribute("TransferedSize", QString::number(TransferedSize));

	return dom;
}

FileTransfer * FileTransfer::fromDomElement(const QDomElement &dom, FileTransferManager *listener)
{
	FileTransferType Type = static_cast<FileTransferType>(dom.attribute("Type").toULong());
	UinType Contact = static_cast<UinType>(dom.attribute("Contact").toULong());
	QString FileName = dom.attribute("FileName");

	FileTransfer *ft = new FileTransfer(listener, DccUnknow, Type, Contact, FileName);

	ft->GaduFileName = dom.attribute("GaduFileName");
	ft->FileSize = dom.attribute("FileSize").toULong();
	ft->TransferedSize = dom.attribute("TransferedSize").toULong();

	if (ft->FileSize == ft->TransferedSize && ft->FileSize != 0)
		ft->Status = StatusFinished;

	// WTF ???
	emit ft->fileTransferStatusChanged(ft);

	return ft;
}

void FileTransfer::startTimeout()
{
	if (!connectionTimeoutTimer)
	{
		connectionTimeoutTimer = new QTimer();
		connect(connectionTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectionTimeout()));
	}

	connectionTimeoutTimer->start(60000, true);
}

void FileTransfer::cancelTimeout()
{
	if (connectionTimeoutTimer)
	{
		delete connectionTimeoutTimer;
		connectionTimeoutTimer = 0;
	}
}

void FileTransfer::setVersion()
{
	const UserListElement &ule = userlist->byID("Gadu", QString::number(Contact));
	int version = ule.protocolData("Gadu", "Version").toUInt() & 0x0000ffff;

	if (version >= 0x2a)
		Version = Dcc7;
	else
		Version = Dcc6;
}

void FileTransfer::start(StartType startType)
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
		return;

	if (Status != StatusFrozen)
		return;

	setVersion();
	if (Version == DccUnknow)
		return;

	prepareFileInfo();

	if (Type == TypeSend)
	{
		if (config_file.readBoolEntry("Network", "AllowDCC") && dcc_manager->dccEnabled())
		{
			Status = StatusWaitForConnection;
			emit fileTransferStatusChanged(this);
			UserListElement user = userlist->byID("Gadu", QString::number(Contact));

			switch (Version)
			{
				case Dcc6:
					startTimeout();
					dcc_manager->getFileTransferSocket(user.IP("Gadu").ip4Addr(), user.port("Gadu"), config_file.readNumEntry("General", "UIN"), user.ID("Gadu").toUInt(), this);
					break;

				case Dcc7:
					Socket = new DccSocket(gg_dcc7_send_file(gadu->session(), Contact, FileName, unicode2cp(FileName), 0)); // last param - hash
					Socket->setHandler(this);
					break;

				default:
					return;
			}
		}
		else
		{
			Status = StatusFrozen;
			emit fileTransferStatusChanged(this);
			emit fileTransferFailed(this, ErrorDccDisabled);
		}
	}
	else
	{
		if (startType != StartRestore)
			return;

		UserListElement ule = userlist->byID("Gadu", QString::number(Contact));
		MessageBox::msg(
			tr("This option only sends a remind message to %1. The transfer will not start immediately.")
			.arg(ule.altNick()));

		UserListElements recv(ule);

		QString message(
			tr("Hello. I am an automatic file-transfer reminder. Could you please send me a file named %1?"));
		if (!gadu->currentStatus().isOffline())
			gadu->sendMessage(recv, message.arg(QUrl(FileName).fileName()));
		if (gadu->seqNum() == -1)
			MessageBox::msg(tr("Error: message was not sent"), false, "Warning");
	}
}

void FileTransfer::stop(StopType stopType)
{
	kdebugf();

	cancelTimeout();
	stopUpdateFileInfo();

	if (Socket)
	{
		Socket->stop();
		Socket = 0;
	}

	Speed = 0;

	if (Status != StatusFinished)
	{
		Status = StatusFrozen;
		emit fileTransferStatusChanged(this);
	}
}

void FileTransfer::connectionTimeout()
{
	kdebugf();

	cancelTimeout();
	stopUpdateFileInfo();

	emit fileTransferStatusChanged(this);
	emit fileTransferFailed(this, ErrorConnectionTimeout);
}

void FileTransfer::connectionError(DccSocket *socket)
{
	kdebugf();

	cancelTimeout();
	stopUpdateFileInfo();

	Status = StatusFrozen;
	emit fileTransferStatusChanged(this);
	emit fileTransferFailed(this, ErrorDccSocketTransfer);
}

void FileTransfer::connectionRejected(DccSocket *socket)
{
	kdebugf();

	cancelTimeout();
	stopUpdateFileInfo();

	Status = StatusRejected;
	emit fileTransferStatusChanged(this);
}

void FileTransfer::addSocket(DccSocket *socket)
{
	kdebugf();

	Socket = socket;
	if (Socket)
	{
		prepareFileInfo();
		startUpdateFileInfo();

		Status = StatusTransfer;
		emit fileTransferStatusChanged(this);
	}
}

void FileTransfer::removeSocket(DccSocket *socket)
{
	kdebugf();

	if (Socket == socket)
		Socket = 0;

	kdebugf2();
}

void FileTransfer::prepareFileInfo()
{
	kdebugf();

	if (!Socket)
		return;

	GaduFileName = cp2unicode(Socket->fileName());
	FileSize = gg_fix32(Socket->fileSize());
	TransferedSize = PrevTransferedSize = gg_fix32(Socket->fileOffset());

	kdebugf2();
}

void FileTransfer::updateFileInfo()
{
	kdebugf();

	if (Status == StatusFinished)
		return;

	if (!Socket)
		return;

	Speed = (Socket->fileOffset() - PrevTransferedSize) / 1024;
	PrevTransferedSize = Socket->fileOffset();

	FileSize = gg_fix32(Socket->fileSize());
	TransferedSize = gg_fix32(Socket->fileOffset());

	Status = StatusTransfer;
	emit fileTransferStatusChanged(this);

	kdebugf2();
}

int FileTransfer::dccType()
{
	kdebugf();

	if (Type == TypeSend)
		return GG_SESSION_DCC_SEND;
	else
		return GG_SESSION_DCC_GET;
}

void FileTransfer::connectionDone(DccSocket *socket)
{
	kdebugf();

	cancelTimeout();
	stopUpdateFileInfo();

	if (Socket)
	{
		FileSize = gg_fix32(Socket->fileSize());
		TransferedSize = gg_fix32(Socket->fileOffset());
	}

	if (TransferedSize == FileSize && FileSize != 0)
		Status = StatusFinished;
	else
		Status = StatusFrozen;

	Speed = 0;

	if (Status == StatusFinished)
		emit fileTransferFinished(this);

	emit fileTransferStatusChanged(this);
}

bool FileTransfer::socketEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	if (socket != Socket)
		return false; // TODO: add assertion

	switch (Socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_FILE_INFO:
			Socket->fillFileInfo(FileName);
			return true;

		default:
			return false;
	}

	return false;
}

void FileTransfer::startUpdateFileInfo()
{
	if (!updateFileInfoTimer)
	{
		updateFileInfoTimer = new QTimer();
		connect(updateFileInfoTimer, SIGNAL(timeout()), this, SLOT(updateFileInfo()));
	}

	updateFileInfoTimer->start(1500);
}

void FileTransfer::stopUpdateFileInfo()
{
	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}
}
