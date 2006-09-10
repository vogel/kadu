/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qsocketnotifier.h>
#include <qvbox.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "action.h"
#include "chat.h"
#include "chat_manager.h"
#include "config_dialog.h"
#include "debug.h"
#include "file_transfer.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "userbox.h"

#include "../notify/notify.h"

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
		if (gadu->currentStatus().isOffline() || !gadu->sendMessage(recv, unicode2cp(message.arg(QUrl(FileName).fileName()))) == -1)
			MessageBox::wrn(tr("Error: message was not sent"));
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

		if (stopType == StopFinally)
			emit fileTransferFinished(this, false);
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
		Socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);

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

void FileTransfer::noneEvent()
{
	kdebugf();
}

void FileTransfer::dccDone()
{
	kdebugf();
}

void FileTransfer::setState()
{
	kdebugf();

	Speed = 0;
	finished();
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

void FileTransfer::finished()
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

	emit fileTransferFinished(this, TransferedSize == FileSize && FileSize != 0);
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

FileTransferListView::FileTransferListView(QWidget *parent, char *name)
	: QListView(parent, name)
{
}

void FileTransferListView::keyPressEvent(QKeyEvent *e)
{
	e->ignore();
}

FileTransferListViewItem::FileTransferListViewItem(QListView *parent, FileTransfer *ft)
	: QObject(parent), QListViewItem(parent), ft(ft)
{
	kdebugf();

	ft->addListener(this, true);

	QUrl url(ft->fileName());

	UserListElement ule = userlist->byID("Gadu", QString::number(ft->contact()));
	setText(0, ule.altNick());
	setText(1, url.fileName());
	setText(5, ft->fileName());

	newFileTransfer(ft);
}

FileTransferListViewItem::~FileTransferListViewItem()
{
	kdebugf();

	if (ft)
		ft->removeListener(this, true);
}

void FileTransferListViewItem::keyPressEvent(QKeyEvent *e)
{
	e->ignore();
}

FileTransfer * FileTransferListViewItem::fileTransfer()
{
	return ft;
}

void FileTransferListViewItem::newFileTransfer(FileTransfer *)
{
	fileTransferStatusChanged(ft);
}

void FileTransferListViewItem::fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)
{
	setText(2, tr("Error"));
}

void FileTransferListViewItem::fileTransferStatusChanged(FileTransfer *ft)
{
	switch (ft->status())
	{
		case FileTransfer::StatusFrozen:
			setText(2, tr("Frozen"));
			break;
		case FileTransfer::StatusWaitForConnection:
			setText(2, tr("Wait for connection"));
			break;
		case FileTransfer::StatusTransfer:
			setText(2, tr("Transfer"));
			break;
		case FileTransfer::StatusFinished:
			setText(2, tr("Finished"));
			break;
	}

	setText(3, QString::number(ft->speed()) + " kB/s");
	setText(4, QString::number(ft->percent()) + " %");
}

void FileTransferListViewItem::fileTransferFinished(FileTransfer *, bool)
{
	setText(2, tr("Finished"));
	setText(3, "");
	setText(4, "100%");
}

void FileTransferListViewItem::fileTransferDestroying(FileTransfer *)
{
	ft = 0;
	deleteLater();
}

FileTransferWindow::FileTransferWindow(QWidget *parent, const char *name)
	: QSplitter(Qt::Vertical, parent, name),
	incomingBox(0), outgoingBox(0), incoming(0), outgoing(0),
	currentListViewItem(0), popupMenu(0), startMenuId(0),
	stopMenuId(0), removeMenuId(0)
{
	kdebugf();

	setWFlags(Qt::WDestructiveClose);

	setCaption(tr("Kadu - file transfers"));

	incomingBox = new QVBox(this);
	incomingBox->setSpacing(5);
	incomingBox->setMargin(2);

	new QLabel(tr("Incoming transfers:"), incomingBox);
	incoming = new FileTransferListView(incomingBox);
	incoming->addColumn(tr("Contact"));
	incoming->addColumn(tr("File name"));
	incoming->addColumn(tr("Status"));
	incoming->addColumn(tr("Speed"));
	incoming->addColumn(tr("Progress"));
	incoming->addColumn(tr("Full file name"));

	connect(incoming, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(listItemClicked(QListViewItem *, const QPoint &, int)));

	outgoingBox = new QVBox(this);
	outgoingBox->setSpacing(5);
	outgoingBox->setMargin(2);

	new QLabel(tr("Outgoing transfers:"), outgoingBox);
	outgoing = new FileTransferListView(outgoingBox);
	outgoing->addColumn(tr("Contact"));
	outgoing->addColumn(tr("File name"));
	outgoing->addColumn(tr("Status"));
	outgoing->addColumn(tr("Speed"));
	outgoing->addColumn(tr("Progress"));
	outgoing->addColumn(tr("Full file name"));

	connect(outgoing, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(listItemClicked(QListViewItem *, const QPoint &, int)));

	popupMenu = new QPopupMenu(this);
	startMenuId = popupMenu->insertItem(tr("Start"), this, SLOT(startTransferClicked()));
	stopMenuId = popupMenu->insertItem(tr("Stop"), this, SLOT(stopTransferClicked()));
	removeMenuId = popupMenu->insertItem(tr("Remove"), this, SLOT(removeTransferClicked()));
	removeCompletedMenuId = popupMenu->insertItem(tr("Remove completed"), this, SLOT(removeCompletedClicked()));

	loadGeometry(this, "General", "TransferWindowGeometry", 200, 200, 500, 300);

	QValueList<int> splitsizes;
	splitsizes.append(config_file.readNumEntry("General", "IncomingTransfersHeight", -1));
	splitsizes.append(config_file.readNumEntry("General", "OutgoingTransfersHeight"));

	if (splitsizes[0] != -1)
		setSizes(splitsizes);

	CONST_FOREACH(i, FileTransfer::AllTransfers)
	{
		(*i)->addListener(this, true);
		newFileTransfer(*i);
	}

	kdebugf2();
}

FileTransferWindow::~FileTransferWindow()
{
	kdebugf();

	CONST_FOREACH(i, FileTransfer::AllTransfers)
		(*i)->removeListener(this, true);

	disconnect(incoming, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(listItemClicked(QListViewItem *, const QPoint &, int)));
	disconnect(outgoing, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(listItemClicked(QListViewItem *, const QPoint &, int)));

	if (config_file.readBoolEntry("General", "SaveGeometry"))
	{
		saveGeometry(this, "General", "TransferWindowGeometry");
		config_file.writeEntry("General", "IncomingTransfersHeight", incomingBox->size().height());
		config_file.writeEntry("General", "OutgoingTransfersHeight", outgoingBox->size().height());
	}

	kdebugf2();
}

void FileTransferWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		hide();
		e->accept();
	}
	else if (e->key() == Qt::Key_Delete)
	{
		if (incoming->hasFocus())
			currentListViewItem = dynamic_cast<FileTransferListViewItem *>(incoming->selectedItem());
		else if (outgoing->hasFocus())
			currentListViewItem = dynamic_cast<FileTransferListViewItem *>(outgoing->selectedItem());
		else
			currentListViewItem = 0;
		removeTransferClicked();
		e->accept();
	}
	else
		QSplitter::keyPressEvent(e);
}

void FileTransferWindow::listItemClicked(QListViewItem *lvi, const QPoint &pos, int)
{
	if (!lvi)
		return;

	if (gadu->currentStatus().isOffline())
	{
		popupMenu->setItemEnabled(startMenuId, false);
		popupMenu->setItemEnabled(stopMenuId, false);
	}

	currentListViewItem = dynamic_cast<FileTransferListViewItem *>(lvi);

	switch (currentListViewItem->fileTransfer()->status())
	{
		case FileTransfer::StatusFrozen:
			popupMenu->setItemEnabled(startMenuId, DccSocket::count() < 8);
			popupMenu->setItemEnabled(stopMenuId, false);
			break;

		case FileTransfer::StatusWaitForConnection:
		case FileTransfer::StatusTransfer:
			popupMenu->setItemEnabled(startMenuId, false);
			popupMenu->setItemEnabled(stopMenuId, true);
			break;

		case FileTransfer::StatusFinished:
			popupMenu->setItemEnabled(startMenuId, false);
			popupMenu->setItemEnabled(stopMenuId, false);
			break;
	}

	popupMenu->popup(pos);
}

void FileTransferWindow::startTransferClicked()
{
	if (!currentListViewItem)
		return;

	currentListViewItem->fileTransfer()->start(FileTransfer::StartRestore);
}

void FileTransferWindow::stopTransferClicked()
{
	if (!currentListViewItem)
		return;

	currentListViewItem->fileTransfer()->stop();
}

void FileTransferWindow::removeTransferClicked()
{
	if (!currentListViewItem)
		return;

	FileTransfer *ft = currentListViewItem->fileTransfer();

	if (ft->status() != FileTransfer::StatusFinished)
		if (!MessageBox::ask("Are you sure you want to remove this transfer?"))
			return;
		else
			ft->stop(FileTransfer::StopFinally);

 	currentListViewItem = 0;

	delete ft;
	setActiveWindow();
}

void FileTransferWindow::removeCompletedClicked()
{
	FOREACH(i, FileTransfer::AllTransfers)
		if ((*i)->status() == FileTransfer::StatusFinished)
			(*i)->deleteLater();
}

void FileTransferWindow::newFileTransfer(FileTransfer *ft)
{
	kdebugf();

	if (ft->type() == FileTransfer::TypeSend)
		new FileTransferListViewItem(outgoing, ft);
	else
		new FileTransferListViewItem(incoming, ft);
}

void FileTransferWindow::fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)
{
}

void FileTransferWindow::fileTransferStatusChanged(FileTransfer *)
{
}

void FileTransferWindow::fileTransferFinished(FileTransfer *fileTransfer, bool ok)
{
}

void FileTransferWindow::fileTransferDestroying(FileTransfer *)
{
}

FileTransferManager::FileTransferManager(QObject *parent, const char *name) : QObject(parent, name),
	fileTransferWindow(0), toggleFileTransferWindowMenuId(0)
{
	kdebugf();
	config_file.addVariable("Network", "LastUploadDirectory", QString(getenv("HOME")) + '/');
	config_file.addVariable("Network", "LastDownloadDirectory", QString(getenv("HOME")) + '/');

	UserBox::userboxmenu->addItemAtPos(1, "SendFile", tr("Send file"),
		this, SLOT(sendFile()),
		HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));
	connect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	Action* send_file_action = new Action(icons_manager->loadIcon("SendFile"),
		tr("Send file"), "sendFileAction");
	connect(send_file_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendFileActionActivated(const UserGroup*)));
	KaduActions.insert("sendFileAction", send_file_action);

	connect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatCreated(const UserGroup *)));
	connect(chat_manager, SIGNAL(chatDestroying(const UserGroup *)), this, SLOT(chatDestroying(const UserGroup *)));

	FOREACH(it, chat_manager->chats())
		handleCreatedChat(*it);

	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(needFileAccept(DccSocket*)),
		this, SLOT(needFileAccept(DccSocket*)));
	connect(dcc_manager, SIGNAL(needFileInfo(DccSocket*)),
		this, SLOT(needFileInfo(DccSocket*)));
	connect(dcc_manager, SIGNAL(noneEvent(DccSocket*)),
		this, SLOT(noneEvent(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccDone(DccSocket*)),
		this, SLOT(dccDone(DccSocket*)));
	connect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));

	QPopupMenu *mainMenu = kadu->mainMenu();
	toggleFileTransferWindowMenuId = mainMenu->insertItem(tr("Toggle transfers window"),
		this, SLOT(toggleFileTransferWindow()), 0, -1, 10);

	notify->registerEvent("fileTransferIncomingFile",  QT_TRANSLATE_NOOP("@default", "An user wants to send you a file"));

	readFromConfig();

	kdebugf2();
}

FileTransferManager::~FileTransferManager()
{
	kdebugf();

	writeToConfig();

	notify->unregisterEvent("fileTransferIncomingFile");

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox::userboxmenu->removeItem(sendfile);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	KaduActions.remove("sendFileAction");

	disconnect(chat_manager, SIGNAL(chatCreated(const UserGroup *)), this, SLOT(chatCreated(const UserGroup *)));
	disconnect(chat_manager, SIGNAL(chatDestroying(const UserGroup *)), this, SLOT(chatDestroying(const UserGroup *)));

	FOREACH(it, chat_manager->chats())
		handleDestroyingChat(*it);

	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(needFileAccept(DccSocket*)),
		this, SLOT(needFileAccept(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(needFileInfo(DccSocket*)),
		this, SLOT(needFileInfo(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(noneEvent(DccSocket*)),
		this, SLOT(noneEvent(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccDone(DccSocket*)),
		this, SLOT(dccDone(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));

	FileTransfer::destroyAll();

	QPopupMenu *mainMenu = kadu->mainMenu();
	mainMenu->removeItem(toggleFileTransferWindowMenuId);

	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		delete fileTransferWindow;
	}

	kdebugf2();
}

void FileTransferManager::readFromConfig()
{
	kdebugf();

	FileTransfer::destroyAll();
	QDomElement fts_elem = xml_config_file->findElement(xml_config_file->rootElement(), "FileTransfers");
	if (fts_elem.isNull())
		return;

	QDomNodeList ft_list = fts_elem.elementsByTagName("FileTransfer");
	FileTransfer *ft;
	for (unsigned int i = 0; i < ft_list.count(); i++)
	{
		ft = FileTransfer::fromDomElement(ft_list.item(i).toElement(), this);
		connect(ft, SIGNAL(fileTransferFinished(FileTransfer *, bool)), this, SLOT(fileTransferFinishedSlot(FileTransfer *, bool)));
	}

	kdebugf2();
}

void FileTransferManager::writeToConfig()
{
	kdebugf();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement fts_elem = xml_config_file->accessElement(root_elem, "FileTransfers");
	xml_config_file->removeChildren(fts_elem);
	CONST_FOREACH(i, FileTransfer::AllTransfers)
		(*i)->toDomElement(fts_elem);
	xml_config_file->sync();

	kdebugf2();
}

void FileTransferManager::sendFile(UinType receiver, const QString &filename)
{
	kdebugf();

	FileTransfer * ft = FileTransfer::search(FileTransfer::TypeSend, receiver, filename);
	if (!ft)
		ft = new FileTransfer(this, FileTransfer::TypeSend, receiver, filename);

	if (!fileTransferWindow)
		toggleFileTransferWindow();

	ft->start();

	kdebugf2();
}

QString FileTransferManager::selectFileToSend()
{
	kdebugf();

	QString f;
	QFileInfo fi;
	do
	{
		f = QFileDialog::getOpenFileName(
			config_file.readEntry("Network", "LastUploadDirectory"),
			QString::null, 0, "open file", tr("Select file location"));
		fi.setFile(f);
		if (f != QString::null && !fi.isReadable())
			MessageBox::msg(tr("This file is not readable"), true);
	}
	while (f != QString::null && !fi.isReadable());
	if (f != QString::null && fi.isReadable())
		config_file.writeEntry("Network", "LastUploadDirectory", fi.dirPath() + '/');

	return f;
}

void FileTransferManager::sendFile(UinType receiver)
{
	kdebugf();

	QString f = selectFileToSend();
	if (f.isEmpty())
		return;

	sendFile(receiver, f);

	kdebugf2();
}

void FileTransferManager::sendFile()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	UserListElements users;
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	QString f = selectFileToSend();
	if (f.isEmpty())
		return;

	users = activeUserBox->selectedUsers();
	CONST_FOREACH(i, users)
		if ((*i).usesProtocol("Gadu") && (*i).ID("Gadu") != config_file.readEntry("General", "UIN"))
			sendFile((*i).ID("Gadu").toUInt(), f);

	kdebugf2();
}

void FileTransferManager::userboxMenuPopup()
{
	kdebugf();

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	bool dccEnabled = config_file.readBoolEntry("Network", "AllowDCC");
	bool anyOk = false;

	UserBox *activeUserBox = UserBox::activeUserBox();

	if (dccEnabled && activeUserBox)
	{
		UserListElements users = activeUserBox->selectedUsers();

		CONST_FOREACH(user, users)
			if ((*user).usesProtocol("Gadu") &&
			    (*user).ID("Gadu").toUInt() != config_file.readUnsignedNumEntry("General","UIN"))
			{
				anyOk = true;
				break;
			}
	}

	UserBox::userboxmenu->setItemEnabled(sendfile, anyOk && dccEnabled);
	kdebugf2();
}

void FileTransferManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}


void FileTransferManager::sendFileActionActivated(const UserGroup* users)
{
	kdebugf();
	if (users->count() == 0)
	{
		kdebugf2();
		return;
	}
	QString f = selectFileToSend();
	if (f.isEmpty())
	{
		kdebugf2();
		return;
	}
	CONST_FOREACH(i, *users)
		if ((*i).usesProtocol("Gadu") && (*i).ID("Gadu") != config_file.readEntry("General", "UIN"))
			sendFile((*i).ID("Gadu").toUInt(), f);
	kdebugf2();
}

void FileTransferManager::chatCreated(const UserGroup *group)
{
	kdebugf();
	Chat* chat = chat_manager->findChat(group);
	handleCreatedChat(chat);
}

void FileTransferManager::chatDestroying(const UserGroup *group)
{
	kdebugf();
	Chat* chat = chat_manager->findChat(group);
	handleDestroyingChat(chat);
}

void FileTransferManager::handleCreatedChat(Chat *chat)
{
	connect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
}

void FileTransferManager::handleDestroyingChat(Chat *chat)
{
	disconnect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
}

void FileTransferManager::fileDropped(const UserGroup *group, const QString &fileName)
{
	CONST_FOREACH(i, *group)
		if ((*i).usesProtocol("Gadu"))
			sendFile((*i).ID("Gadu").toUInt(), fileName);
}

void FileTransferManager::toggleFileTransferWindow()
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: fileTransferWindow:%p\n", fileTransferWindow);
	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		disconnect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		delete fileTransferWindow;
		fileTransferWindow = 0;
	}
	else
	{
		fileTransferWindow = new FileTransferWindow();
		connect(fileTransferWindow, SIGNAL(destroyed()), this, SLOT(fileTransferWindowDestroyed()));
		connect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		fileTransferWindow->show();
	}
	kdebugf2();
}

void FileTransferManager::fileTransferFinishedSlot(FileTransfer *fileTransfer, bool ok)
{
	if (ok && config_file.readBoolEntry("Network", "RemoveCompletedTransfers"))
		fileTransfer->deleteLater();
}

void FileTransferManager::fileTransferWindowDestroyed()
{
	kdebugf();
	fileTransferWindow = 0;
}

void FileTransferManager::connectionBroken(DccSocket* socket)
{
	kdebugf();
	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
	{
		ft->connectionBroken();
		return;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::dccError(DccSocket* socket)
{
	kdebugf();
	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
	{
		ft->dccError();
		return;
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");
	kdebugf2();
}

void FileTransferManager::needFileInfo(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::byUinAndStatus(socket->ggDccStruct()->peer_uin,
		FileTransfer::StatusWaitForConnection);
	if (ft)
	{
		ft->setSocket(socket);
		ft->needFileInfo();
		return;
	}

	kdebugf2();
}

void FileTransferManager::needFileAccept(DccSocket *socket)
{
	QString fileName;
	QString question;
	int answer;

	bool haveFileName = false;
	bool resume = false;

	char fsize[20];
	snprintf(fsize, sizeof(fsize), "%.1f", (float)socket->ggDccStruct()->file_info.size / 1024);

	notify->notify("fileTransferIncomingFile", "Incoming file", userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)));

 	FileTransfer *ft = FileTransfer::search(FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin,
 		cp2unicode(socket->ggDccStruct()->file_info.filename), FileTransfer::FileNameGadu);

	if (ft)
	{
		question = narg(tr("User %1 want to send you a file %2\nof size %3kB.\n"
		                   "This is probably a next part of %4\n What should I do?"),
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
			cp2unicode(socket->ggDccStruct()->file_info.filename),
			QString(fsize),
			ft->fileName()
		);

		answer = QMessageBox::question(0, tr("Incoming transfer"), question,
			tr("Continue transfer"),
			tr("Save file under new name"),
			tr("Ignore transfer")
		);

		switch (answer)
		{
			case 0: // continue
				fileName = ft->fileName();
				haveFileName = true;
				resume = true;
				break;

			case 1: // save under new name
				ft = 0;
				break;

			case 2: // ignore transfer
				kdebugmf(KDEBUG_INFO, "discarded\n");
				socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
				return;
		}
	}
	else
	{
		question = narg(tr("User %1 wants to send us a file %2\nof size %3kB. Accept transfer?"),
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
			cp2unicode(socket->ggDccStruct()->file_info.filename),
			QString(fsize)
		);

		answer = QMessageBox::information(0, tr("Incoming transfer"), question, tr("Yes"), tr("No"),
			QString::null, 0, 1);

		if (answer == 1)
		{
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
			return;
		}
	}

	QFileInfo fi;

	kdebugmf(KDEBUG_INFO, "accepted\n");

	while (true)
	{
		if (!haveFileName || fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(config_file.readEntry("Network", "LastDownloadDirectory")
				+ cp2unicode(socket->ggDccStruct()->file_info.filename),
				QString::null, 0, "save file", tr("Select file location"));

		if (fileName.isEmpty())
		{
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->setState(DCC_SOCKET_TRANSFER_DISCARDED);
			return;
		}

		config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).dirPath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists() && fi.size() < socket->ggDccStruct()->file_info.size)
		{
			question.truncate(0);
			question = tr("File %1 already exists.").arg(fileName);

			switch (QMessageBox::question(0, tr("save file"), question, tr("Overwrite"), tr("Resume"),
			                                 tr("Select another file"), 0, 2))
			{
				case 0:
					resume = false;
					break;

				case 1:
					resume = true;
					break;

				case 2:
					continue;
			}
		}

		haveFileName = false;
		int flags = O_WRONLY;
		if (resume)
			flags |= O_APPEND;
		else
			flags |= O_CREAT | O_TRUNC;

		if ((socket->ggDccStruct()->file_fd = open(fileName.local8Bit().data(), flags, 0600)) == -1)
			MessageBox::wrn(tr("Could not open file. Select another one."));
		else
		{
			socket->ggDccStruct()->offset = fi.size();

 			FileTransfer *ft = FileTransfer::search(FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin,
		 		fileName);

			if (!ft)
				ft = new FileTransfer(this, FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin, fileName);

			ft->setSocket(socket);
			if (!fileTransferWindow)
				toggleFileTransferWindow();

			ft->start();

			break;
		}
	}
}

void FileTransferManager::noneEvent(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
		ft->noneEvent();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::dccDone(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
		ft->dccDone();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::setState(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft != NULL)
	{
		ft->setState();
	}
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

void FileTransferManager::socketDestroying(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft)
		ft->socketDestroying();
	else
		kdebugm(KDEBUG_INFO, "not my socket\n");

	kdebugf2();
}

FileTransferManager* file_transfer_manager = NULL;
