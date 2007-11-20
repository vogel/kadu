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
#include "debug.h"
#include "file_transfer_manager.h"
#include "message_box.h"
#include "protocol.h"

#include "file_transfer.h"

uint32_t gg_fix32(uint32_t);

FileTransfer::FileTransfer(FileTransferManager *listener,
	FileTransferType type, const UinType &contact, const QString &fileName) :
	QObject(0, 0), mainListener(listener), listeners(), Socket(0), Type(type), Status(StatusFrozen),
	Contact(contact), FileName(fileName), GaduFileName(), connectionTimeoutTimer(0),
	updateFileInfoTimer(0), FileSize(0), TransferedSize(0), PrevTransferedSize(0),
	Speed(0), dccFinished(false), direct(false)
{
	kdebugf();

	if (mainListener)
	{
		connectSignals(mainListener, false);

		connect(this, SIGNAL(fileTransferFinished(FileTransfer *, bool)),
			mainListener, SLOT(fileTransferFinishedSlot(FileTransfer *, bool)));
	}

	AllTransfers.insert(AllTransfers.begin(), this);

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

	if (Socket)
		Transfers.remove(Socket);

	AllTransfers.remove(this);

	if (!dccFinished && Socket)
	{
		kdebugmf(KDEBUG_WARNING, "DCC transfer has not finished yet!\n");
		delete Socket;
		Socket = 0;
	}

	if (connectionTimeoutTimer)
	{
		delete connectionTimeoutTimer;
		connectionTimeoutTimer = 0;
	}

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	kdebugf2();
}

FileTransfer * FileTransfer::search(FileTransferType type, const UinType &contact, const QString &fileName,
	FileNameType fileNameType)
{
	kdebugf();

	FOREACH(i, AllTransfers)
		if ((*i)->Type == type && (*i)->Contact == contact)
			if (fileNameType == FileNameFull)
			{
				if ((*i)->FileName == fileName)
					return *i;
			}
			else
			{
				if ((*i)->GaduFileName == fileName)
					return *i;
			}

	return 0;
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
		connect(this, SIGNAL(fileTransferFinished(FileTransfer *, bool)),
			object, SLOT(fileTransferFinished(FileTransfer *, bool)));
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
		connect(this, SIGNAL(fileTransferFinished(FileTransfer *, bool)),
			object, SIGNAL(fileTransferFinished(FileTransfer *, bool)));
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
		disconnect(this, SIGNAL(fileTransferFinished(FileTransfer *, bool)),
			object, SLOT(fileTransferFinished(FileTransfer *, bool)));
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
		disconnect(this, SIGNAL(fileTransferFinished(FileTransfer *, bool)),
			object, SIGNAL(fileTransferFinished(FileTransfer *, bool)));
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

void FileTransfer::start(StartType startType)
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
		return;

	if (Status != StatusFrozen)
		return;

	if (Type == TypeSend)
	{
		if (config_file.readBoolEntry("Network", "AllowDCC") && dcc_manager->dccEnabled())
		{
			if (DccSocket::count() < 8)
			{
				connectionTimeoutTimer = new QTimer();
				connect(connectionTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectionTimeout()));
				connectionTimeoutTimer->start(5000, true);

				UserListElement user = userlist->byID("Gadu", QString::number(Contact));

				DccManager::TryType type = dcc_manager->initDCCConnection(
					user.IP("Gadu").ip4Addr(),
					user.port("Gadu"),
					config_file.readNumEntry("General", "UIN"),
					user.ID("Gadu").toUInt(),
					SLOT(dccSendFile(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
					GG_SESSION_DCC_SEND
				);

				direct = type == DccManager::DIRECT;
				Status = StatusWaitForConnection;
				prepareFileInfo();
				emit fileTransferStatusChanged(this);

				updateFileInfoTimer = new QTimer();
				connect(updateFileInfoTimer, SIGNAL(timeout()), this, SLOT(updateFileInfo()));
				updateFileInfoTimer->start(1000, true);
			}
			else
			{
				Status = StatusFrozen;
				emit fileTransferStatusChanged(this);
				emit fileTransferFailed(this, ErrorDccTooManyConnections);
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
		prepareFileInfo();

		UserListElement ule = userlist->byID("Gadu", QString::number(Contact));
		if (startType == StartRestore)
		{
			MessageBox::msg(
				tr("This option only sends a remind message to %1. The transfer will not start immediately.")
				.arg(ule.altNick()));
		}

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

	if (connectionTimeoutTimer)
	{
		delete connectionTimeoutTimer;
		connectionTimeoutTimer = 0;
	}

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	if (Socket)
	{
		Transfers.remove(Socket);
		delete Socket;
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

	delete connectionTimeoutTimer;
	connectionTimeoutTimer = 0;
	Status = StatusFrozen;

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	emit fileTransferStatusChanged(this);
	emit fileTransferFailed(this, ErrorConnectionTimeout);
}

void FileTransfer::needFileInfo()
{
	kdebugf();

	if (connectionTimeoutTimer)
	{
		delete connectionTimeoutTimer;
		connectionTimeoutTimer = 0;
	}

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	direct = false;

	if (FileName.isEmpty())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Abort transfer\n");
		Socket->discard();

		Status = StatusFrozen;
		emit fileTransferStatusChanged(this);
		emit fileTransferFailed(this, ErrorDccSocketTransfer);

		return;
	}

	gadu->dccFillFileInfo(Socket->ggDccStruct(), FileName);

	Status = StatusTransfer;

	updateFileInfoTimer = new QTimer();
	connect(updateFileInfoTimer, SIGNAL(timeout()), this, SLOT(updateFileInfo()));
	updateFileInfoTimer->start(1000, true);

	emit fileTransferStatusChanged(this);
}

void FileTransfer::connectionBroken()
{
	kdebugf();

	Socket->setState(DCC_SOCKET_TRANSFER_ERROR);

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	Status = StatusFrozen;
	emit fileTransferStatusChanged(this);
	emit fileTransferFailed(this, ErrorDccSocketTransfer);
}

void FileTransfer::dccError()
{
	kdebugf();

	Socket->setState(DCC_SOCKET_TRANSFER_ERROR);

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	Status = StatusFrozen;
	emit fileTransferStatusChanged(this);
	emit fileTransferFailed(this, ErrorDccSocketTransfer);

	if (direct)
	{
		direct = false;
		UserListElement user = userlist->byID("Gadu", QString::number(Contact));
		dcc_manager->initDCCConnection(
			user.IP("Gadu").ip4Addr(),
			user.port("Gadu"),
			config_file.readNumEntry("General", "UIN"),
			user.ID("Gadu").toUInt(),
			SLOT(dccSendFile(uint32_t, uint16_t, UinType, UinType, struct gg_dcc **)),
			GG_SESSION_DCC_SEND, true
		);
	}
}

void FileTransfer::setSocket(DccSocket *socket)
{
	kdebugf();

	if (Socket)
		if (Transfers.contains(Socket))
			Transfers.remove(Socket);

	Socket = socket;

	if (Socket)
	{
		Transfers.insert(Socket, this);

		GaduFileName = cp2unicode(socket->ggDccStruct()->file_info.filename);
		prepareFileInfo();

		if (!updateFileInfoTimer)
		{
			updateFileInfoTimer = new QTimer();
			connect(updateFileInfoTimer, SIGNAL(timeout()), this, SLOT(updateFileInfo()));
		}

		updateFileInfoTimer->start(1000, true);

		Status = StatusTransfer;
		emit fileTransferStatusChanged(this);
	}
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

void FileTransfer::finished(bool successfull)
{
	kdebugf();

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	dccFinished = true;

	if (Socket)
	{
		FileSize = gg_fix32(Socket->ggDccStruct()->file_info.size);
		TransferedSize = gg_fix32(Socket->ggDccStruct()->offset);
	}

	if (TransferedSize == FileSize && FileSize != 0)
		Status = StatusFinished;
	else
		Status = StatusFrozen;

	Speed = 0;

	successfull = successfull && TransferedSize == FileSize && FileSize != 0;

	emit fileTransferFinished(this, successfull);
	emit fileTransferStatusChanged(this);
}

void FileTransfer::prepareFileInfo()
{
	kdebugf();

	if (!Socket)
		return;

	FileSize = gg_fix32(Socket->ggDccStruct()->file_info.size);
	TransferedSize = PrevTransferedSize = gg_fix32(Socket->ggDccStruct()->offset);

	emit fileTransferStatusChanged(this);
	kdebugf2();
}

void FileTransfer::updateFileInfo()
{
	kdebugf();

	if (Status == StatusFinished)
		return;

	if (!Socket)
	{
		if (updateFileInfoTimer)
			updateFileInfoTimer->start(1000, true);
		return;
	}

	Speed = (Socket->ggDccStruct()->offset - PrevTransferedSize) / 1024;
	PrevTransferedSize = Socket->ggDccStruct()->offset;

	FileSize = gg_fix32(Socket->ggDccStruct()->file_info.size);
	TransferedSize = gg_fix32(Socket->ggDccStruct()->offset);

	Status = StatusTransfer;
	emit fileTransferStatusChanged(this);

	updateFileInfoTimer->start(1000, true);

	kdebugf2();
}

void FileTransfer::socketDestroying()
{
	kdebugf();

	if (updateFileInfoTimer)
	{
		delete updateFileInfoTimer;
		updateFileInfoTimer = 0;
	}

	if (Socket)
	{
		FileSize = gg_fix32(Socket->ggDccStruct()->file_info.size);
		TransferedSize = gg_fix32(Socket->ggDccStruct()->offset);
	}

	setSocket(0);
	if (Status != StatusFinished)
	{
		if (FileSize == TransferedSize && FileSize != 0)
			Status = StatusFinished;
		else
			Status = StatusFrozen;

		emit fileTransferStatusChanged(this);
	}
}

FileTransfer* FileTransfer::bySocket(DccSocket* socket)
{
	kdebugf();

	if (Transfers.contains(socket))
		return Transfers[socket];
	else
		return 0;
}

FileTransfer * FileTransfer::byUin(UinType uin)
{
	kdebugf();

	FOREACH(i, AllTransfers)
		if ((*i)->Contact == uin && (*i)->Socket == 0)
			return *i;

	return 0;
}

FileTransfer * FileTransfer::byUinAndStatus(UinType uin, FileTransferStatus status)
{
	kdebugf();

	FOREACH(i, AllTransfers)
		if ((*i)->Contact == uin && (*i)->Socket == 0 && (*i)->Status == status)
			return *i;

	return 0;
}

void FileTransfer::destroyAll()
{
	kdebugf();
	while (!AllTransfers.empty())
	{
		FileTransfer *ft = AllTransfers[0];
		AllTransfers.pop_front();
		delete ft;
	}
	kdebugf2();
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

	FileTransfer *ft = new FileTransfer(listener, Type, Contact, FileName);
	ft->GaduFileName = dom.attribute("GaduFileName");
	ft->FileSize = dom.attribute("FileSize").toULong();
	ft->TransferedSize = dom.attribute("TransferedSize").toULong();

	if (ft->FileSize == ft->TransferedSize && ft->FileSize != 0)
		ft->Status = StatusFinished;

	emit ft->fileTransferStatusChanged(ft);

	return ft;
}

QValueList<FileTransfer *> FileTransfer::AllTransfers;
QMap<DccSocket*, FileTransfer*> FileTransfer::Transfers;
