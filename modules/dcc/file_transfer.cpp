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
#include "file_transfer_notifications.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"
#include "userbox.h"

#include "../notify/notify.h"

/**
 * @ingroup dcc
 * @{
 */
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

FileTransferWidget::FileTransferWidget(QWidget *parent, FileTransfer *ft)
	: QFrame(parent), ft(ft)
{
	kdebugf();

	ft->addListener(this, true);

	setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	QGridLayout *layout = new QGridLayout(this, 3, 4, 2);
	layout->setMargin(10);
	layout->setColStretch(0, 1);
	layout->setColStretch(1, 20);
	layout->setColStretch(2, 20);

	QLabel *icon = new QLabel(this);
	icon->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addMultiCellWidget(icon, 0, 2, 0, 0);

	description = new QLabel(this);
	description->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	description->setScaledContents(true);
	layout->addMultiCellWidget(description, 0, 0, 1, 2);

	progress = new QProgressBar(100, this);
	progress->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addMultiCellWidget(progress, 1, 1, 1, 2);

	status = new QLabel(this);
	status->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	layout->addWidget(status, 2, 1);

	QHBox *buttons = new QHBox(this);
	buttons->setBackgroundMode(Qt::PaletteBase, Qt::PaletteBase);
	buttons->setSpacing(2);
	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	pauseButton = new QPushButton(tr("Pause"), buttons);
	pauseButton->hide();
	connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseTransfer()));

	continueButton = new QPushButton(tr("Continue"), buttons);
	continueButton->hide();
	connect(continueButton, SIGNAL(clicked()), this, SLOT(continueTransfer()));

	QPushButton *deleteThis = new QPushButton(tr("Remove"), buttons);
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(remove()));

	UserListElement ule = userlist->byID("Gadu", QString::number(ft->contact()));

	QUrl url(ft->fileName());

	if (ft->type() == FileTransfer::TypeSend)
	{
		icon->setPixmap(icons_manager->loadIcon("FileTransferSend"));
		description->setText(tr("<b>File</b> %1 <b>to</b> %2").arg(url.fileName()).arg(ule.altNick()));
	}
	else
	{
		icon->setPixmap(icons_manager->loadIcon("FileTransferReceive"));
		description->setText(tr("<b>File</b> %1 <b>from</b> %2").arg(url.fileName()).arg(ule.altNick()));
	}

	fileTransferStatusChanged(ft);

	show();
}

FileTransferWidget::~FileTransferWidget()
{
	kdebugf();

	if (ft)
		ft->removeListener(this, true);
}

FileTransfer * FileTransferWidget::fileTransfer()
{
	kdebugf();

	return ft;
}

void FileTransferWidget::remove()
{
	kdebugf();

	if (ft->status() != FileTransfer::StatusFinished)
		if (!MessageBox::ask("Are you sure you want to remove this transfer?"))
			return;
		else
			ft->stop(FileTransfer::StopFinally);

	// it will destroy widget too, see FileTransferWidget::fileTransferDestroying
	delete ft;
}

void FileTransferWidget::pauseTransfer()
{
	ft->stop();
}

void FileTransferWidget::continueTransfer()
{
	ft->start(FileTransfer::StartRestore);
}

void FileTransferWidget::newFileTransfer(FileTransfer *)
{
	kdebugf();
}

void FileTransferWidget::fileTransferFailed(FileTransfer *, FileTransfer::FileTransferError)
{
	kdebugf();

	status->setText(tr("<b>Error</b>"));

	pauseButton->hide();
	continueButton->show();
}

void FileTransferWidget::fileTransferStatusChanged(FileTransfer *ft)
{
	progress->setProgress(ft->percent());

	switch (ft->status())
	{
		case FileTransfer::StatusFrozen:
			status->setText(tr("<b>Frozen</b>"));
			pauseButton->hide();
			continueButton->show();
			break;
		case FileTransfer::StatusWaitForConnection:
			status->setText(tr("<b>Wait for connection</b>"));
			break;
		case FileTransfer::StatusTransfer:
			status->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(ft->speed())));
			pauseButton->show();
			continueButton->hide();
			break;
		case FileTransfer::StatusFinished:
			status->setText(tr("<b>Finished</b>"));
			break;

		default:
			pauseButton->hide();
			continueButton->hide();
	}
}

void FileTransferWidget::fileTransferFinished(FileTransfer *, bool)
{
	kdebugf();

	progress->setProgress(ft->percent());

	status->setText(tr("Finished"));

	pauseButton->hide();
	continueButton->hide();
}

void FileTransferWidget::fileTransferDestroying(FileTransfer *)
{
	kdebugf();

	ft = 0;
	deleteLater();
}

FileTransferWindow::FileTransferWindow(QWidget *parent, const char *name)
	: QFrame(parent, name)
{
	kdebugf();

	setMinimumSize(QSize(100, 100));

	setWFlags(Qt::WDestructiveClose);

	setCaption(tr("Kadu - file transfers"));

	QGridLayout *mainGrid = new QGridLayout(this, 1, 1);
	mainGrid->setSpacing(2);
	mainGrid->setMargin(2);

	scrollView = new QScrollView(this);
	scrollView->setResizePolicy(QScrollView::AutoOneFit);

	mainGrid->addWidget(scrollView, 0, 0);
	scrollView->move(0, 0);

	frame = new QFrame(scrollView->viewport());

 	frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	transfersLayout = new QVBoxLayout(frame, 0, 1);
	transfersLayout->setDirection(QBoxLayout::Up);

	scrollView->addChild(frame, 0, 0);

	QHBox *buttonBox = new QHBox(this);
	buttonBox->setMargin(2);
	buttonBox->setSpacing(2);

	mainGrid->addWidget(buttonBox, 1, 0, Qt::AlignRight);

	QPushButton *cleanButton = new QPushButton(tr("Clean"), buttonBox);
	connect(cleanButton, SIGNAL(clicked()), this, SLOT(clearClicked()));

	QPushButton *hideButton = new QPushButton(tr("Hide"), buttonBox);
	connect(hideButton, SIGNAL(clicked()), this, SLOT(close()));

	loadGeometry(this, "General", "TransferWindowGeometry", 200, 200, 500, 300);

	CONST_FOREACH(i, FileTransfer::AllTransfers)
	{
		(*i)->addListener(this, true);
		newFileTransfer(*i);
	}

	contentsChanged();

	kdebugf2();
}

FileTransferWindow::~FileTransferWindow()
{
	kdebugf();

	CONST_FOREACH(i, FileTransfer::AllTransfers)
		(*i)->removeListener(this, true);

	saveGeometry(this, "General", "TransferWindowGeometry");

	kdebugf2();
}

void FileTransferWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		close();
		e->accept();
	}
	else
		QFrame::keyPressEvent(e);
}

void FileTransferWindow::clearClicked()
{
	FOREACH(i, FileTransfer::AllTransfers)
		if ((*i)->status() == FileTransfer::StatusFinished)
			(*i)->deleteLater();
}

void FileTransferWindow::contentsChanged()
{
	kdebugf();

	QSize boxSize = frame->sizeHint();

	frame->setMinimumHeight(boxSize.height());
	frame->setMaximumHeight(boxSize.height());

	// workaround
	// without this sometimes this scroll to strange positions
	int y = scrollView->contentsY();
	scrollView->scrollBy(0, -y);
	frame->setGeometry(0, 0, frame->width(), boxSize.height());
	scrollView->scrollBy(0, y);
}

void FileTransferWindow::newFileTransfer(FileTransfer *ft)
{
	kdebugf();

	FileTransferWidget *ftm = new FileTransferWidget(frame, ft);
	transfersLayout->addWidget(ftm);
	map.insert(ft, ftm);

	contentsChanged();
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

void FileTransferWindow::fileTransferDestroying(FileTransfer *ft)
{
	kdebugf();

	if (map.contains(ft))
	{
		transfersLayout->remove(map[ft]);
		map.remove(ft);

		contentsChanged();
	}
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
		tr("Send file"), "sendFileAction", Action::TypeUser);
	connect(send_file_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(sendFileActionActivated(const UserGroup*)));
	KaduActions.insert("sendFileAction", send_file_action);

	connect(chat_manager, SIGNAL(chatCreated(Chat *)), this, SLOT(chatCreated(Chat *)));
	connect(chat_manager, SIGNAL(chatDestroying(Chat *)), this, SLOT(chatDestroying(Chat *)));

	FOREACH(it, chat_manager->chats())
		chatCreated(*it);

	connect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	connect(dcc_manager, SIGNAL(dccEvent(DccSocket*, bool&)),
		this, SLOT(dccEvent(DccSocket*, bool&)));
	connect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
	connect(dcc_manager, SIGNAL(setState(DccSocket*)),
		this, SLOT(setState(DccSocket*)));
	connect(dcc_manager, SIGNAL(socketDestroying(DccSocket*)),
		this, SLOT(socketDestroying(DccSocket*)));

	QPopupMenu *mainMenu = kadu->mainMenu();
	toggleFileTransferWindowMenuId = mainMenu->insertItem(tr("Toggle transfers window"),
		this, SLOT(toggleFileTransferWindow()), 0, -1, 10);

	notification_manager->registerEvent("FileTransfer/IncomingFile",  QT_TRANSLATE_NOOP("@default", "An user wants to send you a file"), CallbackRequired);
	notification_manager->registerEvent("FileTransfer/Finished", QT_TRANSLATE_NOOP("@default", "File transfer was finished"), CallbackNotRequired);

	readFromConfig();

	kdebugf2();
}

FileTransferManager::~FileTransferManager()
{
	kdebugf();

	writeToConfig();

 	notification_manager->unregisterEvent("FileTransfer/IncomingFile");
	notification_manager->unregisterEvent("FileTransfer/Finished");

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox::userboxmenu->removeItem(sendfile);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()), this, SLOT(userboxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));

	KaduActions.remove("sendFileAction");

	disconnect(chat_manager, SIGNAL(chatCreated(Chat *)), this, SLOT(chatCreated(Chat *)));
	disconnect(chat_manager, SIGNAL(chatDestroying(Chat *)), this, SLOT(chatDestroying(Chat *)));

	FOREACH(it, chat_manager->chats())
		chatDestroying(*it);

	disconnect(dcc_manager, SIGNAL(connectionBroken(DccSocket*)),
		this, SLOT(connectionBroken(DccSocket*)));
	disconnect(dcc_manager, SIGNAL(dccEvent(DccSocket*,bool&)),
		this, SLOT(dccEvent(DccSocket*,bool&)));
	disconnect(dcc_manager, SIGNAL(dccError(DccSocket*)),
		this, SLOT(dccError(DccSocket*)));
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

QStringList FileTransferManager::selectFilesToSend()
{
	return QFileDialog::getOpenFileNames(
		QString::null,
		config_file.readEntry("Network", "LastUploadDirectory"),
		0, "open file", tr("Select file location"));
}

void FileTransferManager::sendFile(UinType receiver)
{
	kdebugf();

	QStringList f = selectFilesToSend();
	if (!f.count())
		return;

	CONST_FOREACH(file, f)
		sendFile(receiver, *file);

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

	QStringList f = selectFilesToSend();
	if (!f.count())
		return;

	users = activeUserBox->selectedUsers();
	CONST_FOREACH(i, users)
		CONST_FOREACH(file, f)
			if ((*i).usesProtocol("Gadu") && (*i).ID("Gadu") != config_file.readEntry("General", "UIN"))
				sendFile((*i).ID("Gadu").toUInt(), *file);

	kdebugf2();
}

void FileTransferManager::userboxMenuPopup()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
		return;

	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	bool dccEnabled = config_file.readBoolEntry("Network", "AllowDCC");
	bool dccKeyEnabled = true;

	if (dccEnabled)
	{
		unsigned int myUin = config_file.readUnsignedNumEntry("General", "UIN");
		UserListElements users = activeUserBox->selectedUsers();

		CONST_FOREACH(user, users)
			if (!(*user).usesProtocol("Gadu") || (*user).ID("Gadu").toUInt() == myUin)
			{
				dccKeyEnabled = false;
				break;
			}
	}

	UserBox::userboxmenu->setItemVisible(sendfile, dccKeyEnabled && dccEnabled);
	kdebugf2();
}

void FileTransferManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}


void FileTransferManager::sendFileActionActivated(const UserGroup* users)
{
	if (!users->count())
		return;

	QStringList f = selectFilesToSend();
	if (!f.count())
		return;

	CONST_FOREACH(i, *users)
		CONST_FOREACH(file, f)
			if ((*i).usesProtocol("Gadu") && (*i).ID("Gadu") != config_file.readEntry("General", "UIN"))
				sendFile((*i).ID("Gadu").toUInt(), *file);
	kdebugf2();
}

void FileTransferManager::chatCreated(Chat *chat)
{
	connect(chat, SIGNAL(fileDropped(const UserGroup *, const QString &)),
		this, SLOT(fileDropped(const UserGroup *, const QString &)));
}

void FileTransferManager::chatDestroying(Chat *chat)
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

void FileTransferManager::showFileTransferWindow()
{
	if (!fileTransferWindow)
		toggleFileTransferWindow();
}

void FileTransferManager::toggleFileTransferWindow()
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: fileTransferWindow:%p\n", fileTransferWindow);
	if (fileTransferWindow)
	{
		disconnect(this, SIGNAL(newFileTransfer(FileTransfer *)),
			fileTransferWindow, SLOT(newFileTransfer(FileTransfer *)));
		disconnect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
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
		connect(this, SIGNAL(fileTransferDestroying(FileTransfer *)),
			fileTransferWindow, SLOT(fileTransferDestroying(FileTransfer *)));
		fileTransferWindow->show();
	}
	kdebugf2();
}

void FileTransferManager::fileTransferFinishedSlot(FileTransfer *fileTransfer, bool ok)
{
	QString message;

	if (ok && config_file.readBoolEntry("Network", "RemoveCompletedTransfers"))
		fileTransfer->deleteLater();

	if (ok)
		message = tr("File has been transferred sucessfully.");
	else
		message = tr("File transfer error!");

	Notification *fileTransferFinishedNotification = new Notification("FileTransfer/Finished", "SendFile", UserListElements());
	fileTransferFinishedNotification->setTitle(tr("File transfer finished"));
	fileTransferFinishedNotification->setText(message);

	notification_manager->notify(fileTransferFinishedNotification);
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

void FileTransferManager::dccEvent(DccSocket *socket, bool &lock)
{
	kdebugf();

	switch (socket->ggDccEvent()->type)
	{
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_ACK! uin:%d peer_uin:%d\n",
				socket->ggDccStruct()->uin, socket->ggDccStruct()->peer_uin);
			needFileAccept(socket);
			lock = true;
			break;

		case GG_EVENT_DCC_NEED_FILE_INFO:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_INFO! uin:%d peer_uin:%d\n",
				socket->ggDccStruct()->uin, socket->ggDccStruct()->peer_uin);
			needFileInfo(socket);
			break;

		default:
			break;
	}

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
	kdebugf();

	QString fileName;
	QString question;

	QString fileSize = QString("%1").arg((float)(socket->ggDccStruct()->file_info.size / 1024), 0, 'f', 2);

 	FileTransfer *ft = FileTransfer::search(FileTransfer::TypeReceive, socket->ggDccStruct()->peer_uin,
 		cp2unicode(socket->ggDccStruct()->file_info.filename), FileTransfer::FileNameGadu);

	NewFileTransferNotification *newFileTransferNotification;

 	if (ft)
 	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)), FileTransfer::StartRestore);

 		question = narg(tr("User %1 want to send you a file %2\nof size %3kB.\n"
 		                   "This is probably a next part of %4\n What should I do?"),
 			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
 			cp2unicode(socket->ggDccStruct()->file_info.filename),
			fileSize,
 			ft->fileName()
 		);
	}
	else
	{
		newFileTransferNotification = new NewFileTransferNotification(ft, socket,
			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)), FileTransfer::StartNew);

 		question = narg(tr("User %1 wants to send us a file %2\nof size %3kB. Accept transfer?"),
 			userlist->byID("Gadu", QString::number(socket->ggDccStruct()->peer_uin)).altNick(),
 			cp2unicode(socket->ggDccStruct()->file_info.filename),
			fileSize
		);
	}

	newFileTransferNotification->setText(question);
	newFileTransferNotification->setTitle("Incoming transfer");

	notification_manager->notify(newFileTransferNotification);

	kdebugf2();
}

void FileTransferManager::acceptFile(FileTransfer *ft, DccSocket *socket, QString fileName, bool resume)
{
	kdebugf();

	bool haveFileName = !fileName.isNull();

	QFileInfo fi;

	while (true)
	{
		if (socket == NULL)
		{
			kdebugm(KDEBUG_INFO, "socket is null");
			return;
		}

		if (socket->ggDccStruct() == NULL)
		{
			kdebugm(KDEBUG_INFO, "socket ggDccStruct is null");
			socket->discard();
			return;
		}

		if (!haveFileName || fileName.isEmpty())
			fileName = QFileDialog::getSaveFileName(config_file.readEntry("Network", "LastDownloadDirectory")
				+ cp2unicode(socket->ggDccStruct()->file_info.filename),
				QString::null, 0, "save file", tr("Select file location"));

		if (fileName.isEmpty())
		{
			kdebugmf(KDEBUG_INFO, "discarded\n");
			socket->discard();
			return;
		}

		config_file.writeEntry("Network", "LastDownloadDirectory", QFileInfo(fileName).dirPath() + '/');
		fi.setFile(fileName);

		if (!haveFileName && fi.exists())
		{
			QString question;
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
			showFileTransferWindow();

			ft->start();
			socket->enableNotifiers();

			break;
		}
	}

	kdebugf2();
}

void FileTransferManager::discardFile(DccSocket *socket)
{
	kdebugf();
	socket->discard();
	kdebugf2();
}

void FileTransferManager::setState(DccSocket* socket)
{
	kdebugf();

	FileTransfer *ft = FileTransfer::bySocket(socket);
	if (ft != NULL)
		ft->finished(socket->state() == DCC_SOCKET_TRANSFER_FINISHED);
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

/** @} */

