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
//
#include "kadu.h"
//
#include "dcc.h"
#include "ignore.h"
#include "debug.h"
#include "message_box.h"

QSocketNotifier *dccsnr = NULL;
QSocketNotifier *dccsnw = NULL;
QHostAddress config_dccip;
QHostAddress config_extip;
struct gg_dcc* dccsock;

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
	qApp->postEvent((QObject *)kadu, new QCustomEvent(QEvent::User, me));
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
