/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qlayout.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
//
#include "kadu.h"
//
#include "dcc.h"
#include "ignore.h"
#include "debug.h"
#include "message_box.h"
#include "config_dialog.h"

extern "C" int dcc_init()
{
	kdebugf();
	dcc_manager = new DccManager();
	return 0;
}

extern "C" void dcc_close()
{
	kdebugf();
	delete dcc_manager;
	dcc_manager = NULL;
}

int DccSocket::Count = 0;

DccSocket::DccSocket(struct gg_dcc* dcc_sock)
{
	kdebugf();
	dccsock = dcc_sock;
	dccevent = NULL;
	snr = snw = NULL;
	State = DCC_SOCKET_TRANSFERRING;
	in_watchDcc = false;
	Count++;
	kdebugm(KDEBUG_FUNCTION_END|KDEBUG_INFO, "DccSocket::DccSocket(): dcc sockets count = %d\n", Count);
}

DccSocket::~DccSocket()
{
	kdebugf();
	if (snr)
	{
		snr->setEnabled(false);
		delete snr;
		snr = NULL;
	}
	if (snw)
	{
		snw->setEnabled(false);
		delete snw;
		snw = NULL;
	}
	if (dccevent)
	{
		gadu->freeEvent(dccevent);
		dccevent = NULL;
	}
	if (dccsock)
	{
		if (dccsock->file_fd > 0)
		{
			close(dccsock->file_fd);
		}
		gadu->dccFree(dccsock);
		dccsock = NULL;
		Count--;
	}
	kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "DccSocket::~DccSocket(): dcc sockets count = %d\n", Count);
}

void DccSocket::initializeNotifiers()
{
	kdebugf();
	snr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, this);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dccDataReceived()));

	snw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, this);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dccDataSent()));
	kdebugf2();
}

void DccSocket::dccDataReceived() {
	if (!in_watchDcc)
		watchDcc(GG_CHECK_READ);
}

void DccSocket::dccDataSent() {
	kdebugf();
	snw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc(GG_CHECK_WRITE);
	kdebugf2();
}

void DccSocket::watchDcc(int check)
{
	kdebugf();
	UinsList uins;

	in_watchDcc = true;

	if (!(dccevent = gadu->dccWatchFd(dccsock)))
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): Connection broken unexpectedly!\n");
		connectionBroken();
		return;
	}

	switch (dccevent->type)
	{
		case GG_EVENT_DCC_CLIENT_ACCEPT:
			uins.append(dccsock->peer_uin);
			if (dccsock->uin != (UinType)config_file.readNumEntry("General", "UIN")
				|| !userlist.containsUin(dccsock->peer_uin) || isIgnored(uins))
			tranferDiscarded();
			break;
		case GG_EVENT_NONE:
			noneEvent();
			break;
		case GG_EVENT_DCC_CALLBACK:
			callbackReceived();
			break;
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc():  GG_EVENT_DCC_NEED_FILE_ACK! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			needFileAccept();
			break;
		case GG_EVENT_DCC_NEED_FILE_INFO:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc():  GG_EVENT_DCC_NEED_FILE_INFO! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			needFileInfo();
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): GG_EVENT_DCC_ERROR\n");
			dccError();
			return;
		case GG_EVENT_DCC_DONE:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): GG_EVENT_DCC_DONE\n");
			dccDone();
			return;
		default:
			break;
	}

	dccEvent();

	if (dccsock->check & GG_CHECK_WRITE)
		snw->setEnabled(true);

	if (dccevent)
	{
		gadu->freeEvent(dccevent);
		dccevent = NULL;
	}

	in_watchDcc = false;
	kdebugf2();
}


void DccSocket::setState(int pstate)
{
	kdebugf();
	snr->setEnabled(false);
	snw->setEnabled(false);
	State = pstate;
	
	switch (State)
	{
		case DCC_SOCKET_TRANSFER_FINISHED:
			MessageBox::msg(tr("File has been transferred sucessfully."));
			break;
		case DCC_SOCKET_TRANSFER_DISCARDED:
			break;
		case DCC_SOCKET_TRANSFER_ERROR:
			MessageBox::msg(tr("File transfer error!"));
			break;
		case DCC_SOCKET_CONNECTION_BROKEN:
			break;
		case DCC_SOCKET_COULDNT_OPEN_FILE:
			MessageBox::msg(tr("Couldn't open file!"));
			break;
	}
	deleteLater();

	kdebugf2();
}

int DccSocket::state()
{
	return State;
}

int DccSocket::count()
{
	return Count;
}

void DccSocket::connectionBroken()
{
	kdebugf();
}

void DccSocket::dccError()
{
	kdebugf();
}

void DccSocket::dccEvent()
{
	kdebugf();
}

void DccSocket::needFileAccept()
{
	kdebugf();
}

void DccSocket::needFileInfo()
{
	kdebugf();
}

void DccSocket::noneEvent()
{
	kdebugf();
}

void DccSocket::dccDone()
{
	kdebugf();
	setState(DCC_SOCKET_TRANSFER_FINISHED);
	kdebugf2();
}

void DccSocket::tranferDiscarded()
{
	kdebugf();				
	setState(DCC_SOCKET_TRANSFER_DISCARDED);
	kdebugf2();
}

void DccSocket::callbackReceived()
{
	dcc_manager->cancelTimeout();
	gadu->dccSetType(dccsock, GG_SESSION_DCC_SEND);
}


DccFileDialog::DccFileDialog(FileDccSocket* dccsocket, TransferType type, QDialog* parent, const char* name)
	: QDialog (parent, name), dccsocket(dccsocket), type(type)
{
	kdebugf();
	vlayout1 = new QVBoxLayout(this);
	vlayout1->setAutoAdd(true);
	vlayout1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	dccFinished = false;
	kdebugf2();
}

DccFileDialog::~DccFileDialog()
{
	kdebugf();
	delete time;
	if (!dccFinished)
	{
		kdebugm(KDEBUG_WARNING, "DccFileDialog::closeEvent(): DCC transfer has not finished yet!\n");
		delete dccsocket;
	}
	kdebugf2();
}

void DccFileDialog::closeEvent(QCloseEvent* e)
{
	QDialog::closeEvent(e);
}

void DccFileDialog::printFileInfo(struct gg_dcc* dccsock)
{
	kdebugf();
	long long int percent;
 	long double fpercent;

	QLabel *l_sender = new QLabel(this);

	QString sender;

	if (type == TRANSFER_TYPE_GET)
		sender.append(tr("Sender: "));
	else
		sender.append(tr("Receiver: "));
	sender.append(userlist.byUin(dccsock->peer_uin).altnick);
	l_sender->setText(sender);

	QLabel *l_filename = new QLabel(this);
	sender.truncate(0);

	sender.append(tr("Filename: "));
	sender.append((char *)dccsock->file_info.filename);
	l_filename->setText(sender);

	QLabel *l_filesize = new QLabel(this);
	sender.truncate(0);

	sender.append(tr("File size: "));
	sender.append(QString::number(dccsock->file_info.size));
	sender.append("B");

	l_filesize->setText(sender);

	l_offset = new QLabel(tr("Speed: 0KB/s (not started)  "),this);

	p_progress = new QProgressBar(100, this);
	p_progress->setProgress(0);

	time = new QTime();
	time->start();

	prevOffset = dccsock->offset;
	fpercent = ((long double)dccsock->offset * 100.0) / (long double)dccsock->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent)
	{
		p_progress->setProgress(percent);
		prevPercent = percent;
	}
	else
		p_progress->setProgress(0);

	resize(vlayout1->sizeHint());
	setMinimumSize(vlayout1->sizeHint());
	setFixedHeight(vlayout1->sizeHint().height());

	setCaption(tr("File transfered %1%").arg((int)percent));
	show();
	kdebugf2();
}

void DccFileDialog::updateFileInfo(struct gg_dcc *dccsock)
{
	kdebugf();
	long long int percent;
 	long double fpercent;
	int diffOffset,diffTime;
	QString str;

	if ((diffTime = time->elapsed()) > 1000)
	{
		diffOffset = dccsock->offset - prevOffset;
		prevOffset = dccsock->offset;
		str.append(tr("Speed: "));
		str.append(QString::number(diffOffset/1024));
		str.append("KB/s ");
		if (!diffOffset)
			str.append(tr("(stalled)"));
		l_offset->setText(str);
		time->restart();
	}
	fpercent = ((long double)dccsock->offset * 100.0) / (long double)dccsock->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent)
	{
		p_progress->setProgress(percent);
		prevPercent = percent;
	}
	setCaption(tr("File transfered %1%").arg((int)percent));
	kdebugf2();
}

FileDccSocket::FileDccSocket(struct gg_dcc* dcc_sock)
	: DccSocket(dcc_sock)
{
	filedialog = NULL;
}

FileDccSocket::~FileDccSocket()
{
	kdebugf();
	if (filedialog)
	{
		if (filedialog->dccFinished)
		{
			if (filedialog->isVisible())
				filedialog->close();
			else
				delete filedialog;
			filedialog = NULL;
		}
	}
	kdebugf2();
}

QString FileDccSocket::selectFile()
{
	kdebugf();
	QString f;
	QFileInfo fi;
	do
	{
		f = QFileDialog::getOpenFileName((char *)dccsock->file_info.filename,
			QString::null, 0, tr("open file"), tr("Select file location"));
		fi.setFile(f);
	}
	while (f != QString::null && !fi.isReadable());
	kdebugf2();
	return f;
}

void FileDccSocket::connectionBroken()
{
	kdebugf();
	setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
	kdebugf2();
}

void FileDccSocket::dccError()
{
	kdebugf();
	setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
	kdebugf2();
}

void FileDccSocket::needFileInfo()
{
	kdebugf();
	QString f = selectFile();
	if (f == QString::null)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): Abort transfer\n");
		setState(DCC_SOCKET_TRANSFER_DISCARDED);
		return;
	}
	gadu->dccFillFileInfo(dccsock, f);
	filedialog = new DccFileDialog(this, DccFileDialog::TRANSFER_TYPE_SEND, NULL, "dcc_file_dialog");
	filedialog->printFileInfo(dccsock);
	kdebugf2();
}

void FileDccSocket::needFileAccept()
{
	kdebugf();

	QString str, f;
	QFileInfo fi;

	str.append(tr("User "));
	str.append(userlist.byUin(dccsock->peer_uin).altnick);
	str.append(tr(" wants to send us a file "));
	str.append((char *)dccsock->file_info.filename);
	str.append(tr("\nof size "));

	char fsize[20];
	snprintf(fsize, sizeof(fsize), "%.1fkB", (float) dccsock->file_info.size / 1024);
	str.append(fsize);
	str.append(tr(". Accept transfer?"));

	switch (QMessageBox::information(0, tr("Incoming transfer"), str, tr("Yes"), tr("No"),
		QString::null, 0, 1))
	{
		case 0: // Yes?
			kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): accepted\n");
			f = QFileDialog::getSaveFileName((char *)dccsock->file_info.filename,
				QString::null, 0, tr("save file"), tr("Select file location"));
			if (f.isEmpty())
			{
				kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): discarded\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				return;
			}

			fi.setFile(f);
			if (fi.exists() && fi.size() < dccsock->file_info.size)
			{
				str.truncate(0);
				str = QString(tr("File %1 already exists.")).arg(f);
				switch (QMessageBox::information(0, tr("save file"),
					str, tr("Overwrite"), tr("Resume"),
					tr("Cancel"), 0, 2))
				{
					case 0:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): truncating file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
						{
							MessageBox::wrn(tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							kdebugf2();
							return;
						}
						dccsock->offset = 0;
						break;
					case 1:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): appending to file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_APPEND, 0600)) == -1)
						{
							MessageBox::wrn(tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							kdebugf2();
							return;
						}
						dccsock->offset = fi.size();
						break;
					case 2:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): discarded\n");
						setState(DCC_SOCKET_TRANSFER_DISCARDED);
						kdebugf2();
						return;
				}
			}
			else
			{
				kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): creating file %s\n", f.latin1());

				if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT, 0600)) == -1)
				{
					MessageBox::wrn(tr("Could not open file"));
					setState(DCC_SOCKET_COULDNT_OPEN_FILE);
					kdebugf2();
					return;
				}
				dccsock->offset = 0;
			}

			filedialog = new DccFileDialog(this, DccFileDialog::TRANSFER_TYPE_GET, NULL, "dcc_file_dialog");
			filedialog->printFileInfo(dccsock);
			break;
		case 1:
			kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): discarded\n");
			setState(DCC_SOCKET_TRANSFER_DISCARDED);
			break;
	}
	kdebugf2();
}

void FileDccSocket::noneEvent()
{
	kdebugf();
	if (filedialog && filedialog->isVisible())
		filedialog->updateFileInfo(dccsock);
	kdebugf2();
}

void FileDccSocket::dccDone()
{
	kdebugf();
	DccSocket::dccDone();
	if (filedialog && filedialog->isVisible())
		filedialog->updateFileInfo(dccsock);
	kdebugf2();
}

void FileDccSocket::setState(int pstate)
{
	kdebugf();
	DccSocket::setState(pstate);
	if (filedialog)
		filedialog->dccFinished = true;
	kdebugf2();
}

DccManager::DccManager() : QObject(NULL,"dcc_manager")
{
	kdebugf();
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Send file"), "kadu_sendfile", "F8");

	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC enabled"), "AllowDCC", false);
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP autodetection"), "DccIpDetect", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC IP"));
	ConfigDialog::addLineEdit("Network", "DCC IP", QT_TRANSLATE_NOOP("@default", "IP address:"),"DccIP");
	ConfigDialog::addCheckBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding enabled"), "DccForwarding", false);

	ConfigDialog::addVGroupBox("Network", "Network", QT_TRANSLATE_NOOP("@default", "DCC forwarding properties"));
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External IP address:"), "ExternalIP");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "External TCP port:"), "ExternalPort", "0");
	ConfigDialog::addLineEdit("Network", "DCC forwarding properties", QT_TRANSLATE_NOOP("@default", "Local TCP port:"), "LocalPort", "1550");

	ConfigDialog::connectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));

	ConfigDialog::registerSlotOnCreate(this, SLOT(configDialogCreated()));
	ConfigDialog::registerSlotOnApply(this, SLOT(configDialogApply()));

	DccSock = NULL;
	DccSnr = NULL;
	DccSnw = NULL;

	if (!config_file.readBoolEntry("Network","DccIpDetect"))
		if (!ConfigDccIp.setAddress(config_file.readEntry("Network","DccIP", "")))
			ConfigDccIp.setAddress((unsigned int)0);

	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
		config_extip.setAddress((unsigned int)0);

	connect(&TimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));

	connect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	connect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	UserBox::userboxmenu->addItemAtPos(1, "SendFile", tr("Send file"),
		this,SLOT(sendFile()),
		HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));
	connect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userboxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));
	kdebugf2();
}

DccManager::~DccManager()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Network", "DCC enabled", SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	ConfigDialog::disconnectSlot("Network", "DCC IP autodetection", SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));

	ConfigDialog::unregisterSlotOnCreate(this, SLOT(configDialogCreated()));
	ConfigDialog::unregisterSlotOnApply(this, SLOT(configDialogApply()));

	ConfigDialog::removeControl("Network", "DCC enabled");
	ConfigDialog::removeControl("Network", "DCC IP autodetection");

	ConfigDialog::removeControl("Network", "IP address:");
	ConfigDialog::removeControl("Network", "DCC forwarding enabled");
	ConfigDialog::removeControl("Network", "DCC IP");

	ConfigDialog::removeControl("Network", "External IP address:");
	ConfigDialog::removeControl("Network", "External TCP port:");
	ConfigDialog::removeControl("Network", "Local TCP port:");
	ConfigDialog::removeControl("Network", "DCC forwarding properties");

	ConfigDialog::removeControl("ShortCuts", "Send file");

	disconnect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	disconnect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	disconnect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox::userboxmenu->removeItem(sendfile);
	disconnect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userboxMenuPopup()));
	disconnect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));
	kdebugf2();
}

QHostAddress DccManager::configDccIp()
{
	return ConfigDccIp;
}

void DccManager::watchDcc()
{
	kdebugf();
	struct gg_event* dcc_e;
	if (!(dcc_e = gadu->dccWatchFd(DccSock)))
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): Connection broken unexpectedly!\n");
		config_file.writeEntry("Network", "AllowDCC", false);
		delete DccSnr;
		DccSnr = NULL;
		delete DccSnw;
		DccSnw = NULL;
		return;
	}

	switch (dcc_e->type)
	{
		case GG_EVENT_NONE:
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			if (DccSocket::count() < 8)
			{
				FileDccSocket* dcc = new FileDccSocket(dcc_e->event.dcc_new);
				connect(dcc, SIGNAL(dccFinished(DccSocket *)), this, SLOT(dccFinished(DccSocket *)));
				dcc->initializeNotifiers();
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_NEW: spawning object\n");
			}
			else
			{
				if (dcc_e->event.dcc_new->file_fd > 0)
					close(dcc_e->event.dcc_new->file_fd);
				gadu->dccFree(dcc_e->event.dcc_new);
			}
			break;
		default:
			break;
	}

	if (DccSock->check == GG_CHECK_WRITE)
		DccSnw->setEnabled(true);

	gadu->freeEvent(dcc_e);
	kdebugf2();
}

void DccManager::dccFinished(DccSocket* dcc)
{
	kdebugf();
	delete dcc;
	kdebugf2();
}

void DccManager::dccReceived()
{
	kdebugf();
	watchDcc();
	kdebugf2();
}

void DccManager::dccSent()
{
	kdebugf();
	DccSnw->setEnabled(false);
	if (DccSock->check & GG_CHECK_WRITE)
		watchDcc();
	kdebugf2();
}

void DccManager::timeout()
{
	MessageBox::msg(tr("Direct connection timeout!\nThe receiver doesn't support direct connections or\nboth computers are behind routers with nat."));
}

void DccManager::cancelTimeout()
{
	TimeoutTimer.stop();
}

void DccManager::setupDcc()
{
	kdebugf();

	if (!config_file.readBoolEntry("Network", "AllowDCC"))
	{
		kdebugf2();
		return;
	}

	QHostAddress dccIp;

	if (!ConfigDccIp.ip4Addr())
		dccIp.setAddress("255.255.255.255");
	else
		dccIp = ConfigDccIp;

	DccSock = gadu->dccSocketCreate(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550));

	if (!DccSock)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setupDcc(): Couldn't bind DCC socket.\n");
		gadu->dccFree(DccSock);

		QMessageBox::warning(kadu, "",
			tr("Couldn't create DCC socket.\nDirect connections disabled."));
		kdebugf2();
		return;
	}

	gadu->setDccIpAndPort(htonl(dccIp.ip4Addr()), DccSock->port);

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol:setupDcc() DCC_IP=%s DCC_PORT=%d\n", dccIp.toString().latin1(), DccSock->port);

	DccSnr = new QSocketNotifier(DccSock->fd, QSocketNotifier::Read, kadu);
	connect(DccSnr, SIGNAL(activated(int)), this, SLOT(dccReceived()));

	DccSnw = new QSocketNotifier(DccSock->fd, QSocketNotifier::Write, kadu);
	connect(DccSnw, SIGNAL(activated(int)), this, SLOT(dccSent()));

	kdebugf2();
}

void DccManager::closeDcc()
{
	kdebugf();
	if (DccSnr)
	{
		delete DccSnr;
		DccSnr = NULL;
	}

	if (DccSnw)
	{
		delete DccSnw;
		DccSnw = NULL;
	}

	if (DccSock)
	{
		gadu->dccFree(DccSock);
		DccSock = NULL;
		gadu->setDccIpAndPort(0, 0);
	}
	kdebugf2();
}

void DccManager::sendFile()
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (ConfigDccIp.isIp4Addr())
		{
			struct gg_dcc *dcc_new;
			UserBox *activeUserBox=UserBox::getActiveUserBox();
			UserList users;
			if (activeUserBox==NULL)
			{
				kdebugf2();
				return;
			}
			users= activeUserBox->getSelectedUsers();
			if (users.count() != 1)
			{
				kdebugf2();
				return;
			}
			UserListElement user = (*users.begin());
			if (user.port >= 10)
			{
				if ((dcc_new = gadu->dccSendFile(htonl(user.ip.ip4Addr()), user.port,
					config_file.readNumEntry("General", "UIN"), user.uin)) != NULL)
				{
					FileDccSocket* dcc = new FileDccSocket(dcc_new);
					connect(dcc, SIGNAL(dccFinished(DccSocket*)), dcc_manager,
						SLOT(dccFinished(DccSocket*)));
					dcc->initializeNotifiers();
				}
			}
			else
			{
				TimeoutTimer.start(3000, TRUE);
				gadu->dccRequest(user.uin);
			}
		}
	kdebugf2();
}

void DccManager::userboxMenuPopup()
{
	kdebugf();
	int sendfile = UserBox::userboxmenu->getItem(tr("Send file"));
	UserBox* activeUserBox=UserBox::getActiveUserBox();
	if (activeUserBox==NULL)//to siê zdarza...
	{
		kdebugf2();
		return;
	}
	UserList users = activeUserBox->getSelectedUsers();
	UserListElement user = (*users.begin());

	bool containsOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));
	bool userIsOnline=(user.status == GG_STATUS_AVAIL ||
			user.status == GG_STATUS_AVAIL_DESCR ||
			user.status == GG_STATUS_BUSY ||
			user.status == GG_STATUS_BUSY_DESCR);
	bool dccEnabled=(users.count() == 1 &&
		config_file.readBoolEntry("Network", "AllowDCC") &&
		!containsOurUin &&
		userIsOnline &&
		DccSocket::count() < 8);

	UserBox::userboxmenu->setItemEnabled(sendfile, dccEnabled);
	kdebugf2();
}

void DccManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}

void DccManager::ifDccEnabled(bool value)
{
	kdebugf();

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked() && value);
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() && value);
	kdebugf2();
}

void DccManager::ifDccIpEnabled(bool value)
{
	kdebugf();
	ConfigDialog::getVGroupBox("Network", "DCC IP")->setEnabled(!value);
	kdebugf2();
}

void DccManager::configDialogCreated()
{
	kdebugf();
	QCheckBox *b_dccenabled = ConfigDialog::getCheckBox("Network", "DCC enabled");
	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(b_dccenabled->isChecked());
	g_dccip->setEnabled(!b_dccip->isChecked()&& b_dccenabled->isChecked());
	b_dccfwd->setEnabled(b_dccenabled->isChecked());
	g_fwdprop->setEnabled(b_dccenabled->isChecked() && b_dccfwd->isChecked());
	connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	kdebugf2();
}

void DccManager::configDialogApply()
{
	kdebugf();
	if (!ConfigDccIp.setAddress(config_file.readEntry("Network", "DccIP")))
	{
		config_file.writeEntry("Network", "DccIP", "0.0.0.0");
		ConfigDccIp.setAddress((unsigned int)0);
	}
	if (!config_extip.setAddress(config_file.readEntry("Network", "ExternalIP")))
	{
		config_file.writeEntry("Network", "ExternalIP", "0.0.0.0");
		config_extip.setAddress((unsigned int)0);
	}
	if (config_file.readNumEntry("Network", "ExternalPort")<=1023)
		config_file.writeEntry("Network", "ExternalPort", 0);
	kdebugf2();
}

void DccManager::dccConnectionReceived(const UserListElement& sender)
{
	kdebugf();
	struct gg_dcc* dcc_new;
	if (DccSocket::count() < 8)
	{
		dcc_new = gadu->dccGetFile(htonl(sender.ip.ip4Addr()), sender.port, config_file.readNumEntry("General","UIN"), sender.uin);
		if (dcc_new)
		{
			FileDccSocket* dcc = new FileDccSocket(dcc_new);
			connect(dcc, SIGNAL(dccFinished(DccSocket*)), this, SLOT(dccFinished(DccSocket*)));
			dcc->initializeNotifiers();
		}
	}
	kdebugf2();
}

DccManager* dcc_manager = NULL;
