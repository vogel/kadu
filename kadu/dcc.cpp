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

QSocketNotifier* dccsnr = NULL;
QSocketNotifier* dccsnw = NULL;
QHostAddress config_dccip;
struct gg_dcc* dccsock = NULL;

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
		gg_free_event(dccevent);
		dccevent = NULL;
	}
	if (dccsock)
	{
		if (dccsock->file_fd > 0)
		{
			close(dccsock->file_fd);
		}
		gg_dcc_free(dccsock);
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

	if (!(dccevent = gg_dcc_watch_fd(dccsock)))
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
			{
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): strange uins!\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
			}
			break;
		case GG_EVENT_NONE:
			noneEvent();
			break;
		case GG_EVENT_DCC_CALLBACK:
			gg_dcc_set_type(dccsock, GG_SESSION_DCC_SEND);
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
		gg_free_event(dccevent);
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
	DccSocket **me = new (DccSocket *);
	*me = this;
	qApp->postEvent((QObject *)dcc_manager, new QCustomEvent(QEvent::User, me));
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
}

void DccSocket::dccError()
{
}

void DccSocket::dccEvent()
{
}

void DccSocket::needFileAccept()
{
}

void DccSocket::needFileInfo()
{
}

void DccSocket::noneEvent()
{
}

void DccSocket::dccDone()
{
	setState(DCC_SOCKET_TRANSFER_FINISHED);
}

DccFileDialog::DccFileDialog(FileDccSocket* dccsocket, TransferType type, QDialog* parent, const char* name)
	: QDialog (parent, name), dccsocket(dccsocket), type(type)
{
	vlayout1 = new QVBoxLayout(this);
	vlayout1->setAutoAdd(true);
	vlayout1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	dccFinished = false;
}

DccFileDialog::~DccFileDialog()
{
	delete time;
	if (!dccFinished)
	{
		kdebugm(KDEBUG_WARNING, "DccFileDialog::closeEvent(): DCC transfer has not finished yet!\n");
		delete dccsocket;
	}
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
	if (percent > prevPercent) {
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

	if ((diffTime = time->elapsed()) > 1000) {
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
	if (percent > prevPercent) {
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
}

QString FileDccSocket::selectFile()
{
	QString f;
	QFileInfo fi;
	do
	{
		f = QFileDialog::getOpenFileName((char *)dccsock->file_info.filename,
			QString::null, 0, tr("open file"), tr("Select file location"));
		fi.setFile(f);
	}
	while (f != QString::null && !fi.isReadable());
	return f;
}

void FileDccSocket::connectionBroken()
{
	setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
}

void FileDccSocket::dccError()
{
	setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
}

void FileDccSocket::needFileInfo()
{
	QString f = selectFile();
	if (f == QString::null)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "DccSocket::watchDcc(): Abort transfer\n");
		setState(DCC_SOCKET_TRANSFER_DISCARDED);
		return;
	}
	gg_dcc_fill_file_info(dccsock, f.local8Bit());
	filedialog = new DccFileDialog(this, DccFileDialog::TRANSFER_TYPE_SEND, NULL, "dcc_file_dialog");
	filedialog->printFileInfo(dccsock);
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
	snprintf(fsize, sizeof(fsize), "%.1fKB", (float) dccsock->file_info.size / 1024);
	str.append(fsize);
	str.append(tr(". Accept transfer?"));

	switch (QMessageBox::information(0, tr("Incoming transfer"), str, tr("Yes"), tr("No"),
		QString::null, 0, 1)) {
		case 0: // Yes?
			kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): accepted\n");
			f = QFileDialog::getSaveFileName((char *)dccsock->file_info.filename,
				QString::null, 0, tr("save file"), tr("Select file location"));
			if (f.isEmpty()) {
				kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): discarded\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				return;
				}

			fi.setFile(f);
			if (fi.exists() && fi.size() < dccsock->file_info.size) {
				str.truncate(0);
				str = QString(tr("File %1 already exists.")).arg(f);
				switch (QMessageBox::information(0, tr("save file"),
					str, tr("Overwrite"), tr("Resume"),
					tr("Cancel"), 0, 2)) {
					case 0:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): truncating file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
							MessageBox::wrn(tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							return;
							}
						dccsock->offset = 0;
						break;
					case 1:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): appending to file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_APPEND, 0600)) == -1) {
							MessageBox::wrn(tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							return;
							}
						dccsock->offset = fi.size();
						break;
					case 2:
						kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): discarded\n");
						setState(DCC_SOCKET_TRANSFER_DISCARDED);
						return;
					}
				}
			else {
				kdebugm(KDEBUG_INFO, "DccSocket::askAccept(): creating file %s\n", f.latin1());

				if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT, 0600)) == -1) {
					MessageBox::wrn(tr("Could not open file"));
					setState(DCC_SOCKET_COULDNT_OPEN_FILE);
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
	if (filedialog && filedialog->isVisible())
		filedialog->updateFileInfo(dccsock);
}

void FileDccSocket::dccDone()
{
	DccSocket::dccDone();
	if (filedialog && filedialog->isVisible())
		filedialog->updateFileInfo(dccsock);
}

void FileDccSocket::setState(int pstate)
{
	DccSocket::setState(pstate);
	if (filedialog)
		filedialog->dccFinished = true;
}

void DccManager::initModule()
{
	dcc_manager = new DccManager();
}

DccManager::DccManager() : QObject(NULL,"dcc_manager")
{
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

	if (!config_file.readBoolEntry("Network","DccIpDetect"))
		if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP", "")))
			config_dccip.setAddress((unsigned int)0);

	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP", "")))
		config_extip.setAddress((unsigned int)0);

	connect(gadu, SIGNAL(connecting()), this, SLOT(setupDcc()));
	connect(gadu, SIGNAL(disconnected()), this, SLOT(closeDcc()));
	connect(gadu, SIGNAL(dccConnectionReceived(const UserListElement&)),
		this, SLOT(dccConnectionReceived(const UserListElement&)));
	UserBox::userboxmenu->addItemAtPos(1, "SendFile", tr("Send file"),
		this,SLOT(sendFile()),
		HotKey::shortCutFromFile("ShortCuts", "kadu_sendfile"));
	connect(UserBox::userboxmenu,SIGNAL(popup()),this,SLOT(userboxMenuPopup()));
	connect(kadu, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(kaduKeyPressed(QKeyEvent*)));
}

void DccManager::watchDcc()
{
	kdebugf();
	struct gg_event* dcc_e;
	if (!(dcc_e = gg_dcc_watch_fd(dccsock))) {
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): Connection broken unexpectedly!\n");
		config_file.writeEntry("Network", "AllowDCC", false);
		delete dccsnr;
		dccsnr = NULL;
		delete dccsnw;
		dccsnw = NULL;
		return;
		}

	switch (dcc_e->type) {
		case GG_EVENT_NONE:
			break;
		case GG_EVENT_DCC_ERROR:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_ERROR\n");
			break;
		case GG_EVENT_DCC_NEW:
			if (DccSocket::count() < 8) {
				FileDccSocket* dcc = new FileDccSocket(dcc_e->event.dcc_new);
				connect(dcc, SIGNAL(dccFinished(DccSocket *)), this, SLOT(dccFinished(DccSocket *)));
				dcc->initializeNotifiers();
				kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "Kadu::watchDcc(): GG_EVENT_DCC_NEW: spawning object\n");
				}
			else {
				if (dcc_e->event.dcc_new->file_fd > 0)
					close(dcc_e->event.dcc_new->file_fd);
				gg_dcc_free(dcc_e->event.dcc_new);
				}
			break;
		default:
			break;
		}

	if (dccsock->check == GG_CHECK_WRITE)
		dccsnw->setEnabled(true);

	gg_free_event(dcc_e);
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
	dccsnw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc();
	kdebugf2();
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

	if (!config_dccip.ip4Addr())
		dccIp.setAddress("255.255.255.255");
	else
		dccIp = config_dccip;

	dccsock = gg_dcc_socket_create(config_file.readNumEntry("General", "UIN"), config_file.readNumEntry("Network", "LocalPort", 1550));

	if (!dccsock)
	{
		kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol::setupDcc(): Couldn't bind DCC socket.\n");
		gg_dcc_free(dccsock);

		QMessageBox::warning(kadu, "",
			tr("Couldn't create DCC socket.\nDirect connections disabled."));
		return;
	}

	gg_dcc_ip = htonl(dccIp.ip4Addr());
	gg_dcc_port = dccsock->port;

	kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "GaduProtocol:setupDcc() DCC_IP=%s DCC_PORT=%d\n", dccIp.toString().latin1(), dccsock->port);

	dccsnr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, kadu);
	connect(dccsnr, SIGNAL(activated(int)), this, SLOT(dccReceived()));

	dccsnw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, kadu);
	connect(dccsnw, SIGNAL(activated(int)), this, SLOT(dccSent()));

	kdebugf2();
}

void DccManager::closeDcc()
{
	if (dccsnr)
	{
		delete dccsnr;
		dccsnr = NULL;
	}

	if (dccsnw)
	{
		delete dccsnw;
		dccsnw = NULL;
	}

	if (dccsock)
	{
		gg_dcc_free(dccsock);
		dccsock = NULL;
		gg_dcc_ip = 0;
		gg_dcc_port = 0;
	}
}

void DccManager::sendFile()
{
	kdebugf();
	if (config_file.readBoolEntry("Network", "AllowDCC"))
		if (config_dccip.isIp4Addr()) {
			struct gg_dcc *dcc_new;
			UserBox *activeUserBox=UserBox::getActiveUserBox();
			UserList users;
			if (activeUserBox==NULL)
				return;
			users= activeUserBox->getSelectedUsers();
			if (users.count() != 1)
				return;
			UserListElement user = (*users.begin());
			if (user.port >= 10) {
				if ((dcc_new = gg_dcc_send_file(htonl(user.ip.ip4Addr()), user.port,
					config_file.readNumEntry("General", "UIN"), user.uin)) != NULL) {
					FileDccSocket* dcc = new FileDccSocket(dcc_new);
					connect(dcc, SIGNAL(dccFinished(DccSocket*)), dcc_manager,
						SLOT(dccFinished(DccSocket*)));
					dcc->initializeNotifiers();
					}
				}
			else
				gg_dcc_request(sess, user.uin);
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
	if (DccSocket::count() >= 8 && users.count() != 1)
	{
		UserBox::userboxmenu->setItemEnabled(sendfile, false);
	}
	bool isOurUin=users.containsUin(config_file.readNumEntry("General", "UIN"));
	if (users.count() == 1 && (config_file.readBoolEntry("Network", "AllowDCC") &&
		(user.status == GG_STATUS_AVAIL || user.status == GG_STATUS_AVAIL_DESCR ||
		user.status == GG_STATUS_BUSY || user.status == GG_STATUS_BUSY_DESCR)) && !isOurUin)
	{
		UserBox::userboxmenu->setItemEnabled(sendfile, true);
	}
	else
	{
		UserBox::userboxmenu->setItemEnabled(sendfile, false);
	}
	kdebugf2();
}

void DccManager::kaduKeyPressed(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "kadu_sendfile"))
		sendFile();
}

bool DccManager::event(QEvent* e)
{
	QCustomEvent *ce;
	DccSocket *dcc;
	DccSocket **data;

	if (e->type() == QEvent::User)
	{
		kdebugf();
		ce = (QCustomEvent *)e;
		data = (DccSocket **)ce->data();
		dcc = *data;
		switch (dcc->state())
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
		delete data;
		delete dcc;
		ce->setData(NULL);
	}
	return QObject::event(e);
}

void DccManager::ifDccEnabled(bool value)
{
	kdebugf();

	QCheckBox *b_dccip= ConfigDialog::getCheckBox("Network", "DCC IP autodetection");
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	QVGroupBox *g_fwdprop = ConfigDialog::getVGroupBox("Network", "DCC forwarding properties");
	QCheckBox *b_dccfwd = ConfigDialog::getCheckBox("Network", "DCC forwarding enabled");

	b_dccip->setEnabled(value);
	g_dccip->setEnabled(!b_dccip->isChecked()&& value);
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() &&value);
	kdebugf2();
}

void DccManager::ifDccIpEnabled(bool value)
{
	kdebugf();
	QVGroupBox *g_dccip = ConfigDialog::getVGroupBox("Network", "DCC IP");
	g_dccip->setEnabled(!value);
	kdebugf2();
}

void DccManager::configDialogCreated()
{
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
}

void DccManager::configDialogApply()
{
	if (!config_dccip.setAddress(config_file.readEntry("Network","DccIP")))
	{
		config_file.writeEntry("Network","DccIP","0.0.0.0");
		config_dccip.setAddress((unsigned int)0);
	}
	if (!config_extip.setAddress(config_file.readEntry("Network","ExternalIP")))
	{
		config_file.writeEntry("Network","ExternalIP","0.0.0.0");
		config_extip.setAddress((unsigned int)0);
	}
	if (config_file.readNumEntry("Network","ExternalPort")<=1023)
		config_file.writeEntry("Network","ExternalPort",0);
}

void DccManager::dccConnectionReceived(const UserListElement& sender)
{
	kdebugf();
	struct gg_dcc* dcc_new;
	if (DccSocket::count() < 8)
	{
		dcc_new = gg_dcc_get_file(htonl(sender.ip.ip4Addr()), sender.port, config_file.readNumEntry("General","UIN"), sender.uin);
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
