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

QSocketNotifier *dccsnr = NULL;
QSocketNotifier *dccsnw = NULL;
QHostAddress config_dccip;
QHostAddress config_extip;
struct gg_dcc* dccsock;

int dccSocketClass::count = 0;

dccSocketClass::dccSocketClass(struct gg_dcc *dcc_sock, int type) : QObject(), type(type) {
	dccsock = dcc_sock;
	dccevent = NULL;
	snr = snw = NULL;
	state = DCC_SOCKET_TRANSFERRING;
	filedialog = NULL;
	in_watchDcc = false;
	count++;
	kdebug("dccSocketClass::dccSocketClass(): dcc sockets count = %d\n", count);
}

dccSocketClass::~dccSocketClass() {
	kdebug("dccSocketClass::~dccSocketClass\n");
	if (filedialog) {
		if (filedialog->dccFinished) {
			if (filedialog->isVisible())
				filedialog->close();
			else
				delete filedialog;
			filedialog = NULL;
			}
		}
	if (snr) {
		snr->setEnabled(false);
		delete snr;
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		delete snw;
		snw = NULL;
		}
	if (dccevent) {
		gg_free_event(dccevent);
		dccevent = NULL;
		}
	if (dccsock) {
		if (dccsock->file_fd > 0) {
			close(dccsock->file_fd);
			}
		gg_dcc_free(dccsock);
		dccsock = NULL;
		count--;
		}
	kdebug("dccSocketClass::~dccSocketClass(): dcc sockets count = %d\n", count);
}

void dccSocketClass::initializeNotifiers() {
	snr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, this);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dccDataReceived()));

	snw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, this);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dccDataSent()));
}



void dccSocketClass::dccDataReceived() {
	if (!in_watchDcc)
		watchDcc(GG_CHECK_READ);
}

void dccSocketClass::dccDataSent() {
	kdebug("dccSocketClass::dccDataSent()\n");
	snw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc(GG_CHECK_WRITE);
}

void dccSocketClass::watchDcc(int check) {
	QString f;
	UinsList uins;

	in_watchDcc = true;

	kdebug("dccSocketClass::watchDcc()\n");			
	if (!(dccevent = gg_dcc_watch_fd(dccsock))) {
		kdebug("dccSocketClass::watchDcc(): Connection broken unexpectedly!\n");
		connectionBroken();
		if (type == DCC_TYPE_FILE)
			setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
		return;
		}

	switch (dccevent->type) {
		case GG_EVENT_DCC_CLIENT_ACCEPT:
			uins.append(dccsock->peer_uin);
			if (dccsock->uin != config_file.readNumEntry("General", "UIN")
				|| !userlist.containsUin(dccsock->peer_uin) || isIgnored(uins)) {
				kdebug("dccSocketClass::watchDcc(): strange uins!\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				}
			break;
		case GG_EVENT_NONE:
			if (filedialog && filedialog->isVisible())
				filedialog->updateFileInfo(dccsock);
			break;
		case GG_EVENT_DCC_CALLBACK:
			gg_dcc_set_type(dccsock, GG_SESSION_DCC_SEND);
			break;
		case GG_EVENT_DCC_NEED_FILE_ACK:
			kdebug("dccSocketClass::watchDcc():  GG_EVENT_DCC_NEED_FILE_ACK! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			askAccept();
			break;
		case GG_EVENT_DCC_NEED_FILE_INFO:
			kdebug("dccSocketClass::watchDcc():  GG_EVENT_DCC_NEED_FILE_INFO! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			f = selectFile();
			if (f == QString::null) {
				kdebug("dccSocketClass::watchDcc(): Abort transfer\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				return;
				}
			gg_dcc_fill_file_info(dccsock, f.local8Bit());
			filedialog = new DccFileDialog(this, DCC_TYPE_SEND);
			filedialog->printFileInfo(dccsock);
			break;
		case GG_EVENT_DCC_ERROR:
			kdebug("dccSocketClass::watchDcc(): GG_EVENT_DCC_ERROR\n");
			dccError();
			if (type == DCC_TYPE_FILE)
				setState(filedialog ? DCC_SOCKET_TRANSFER_ERROR : DCC_SOCKET_CONNECTION_BROKEN);
			return;
		case GG_EVENT_DCC_DONE:
			kdebug("dccSocketClass::watchDcc(): GG_EVENT_DCC_DONE\n");
			if (filedialog && filedialog->isVisible())
				filedialog->updateFileInfo(dccsock);
			setState(DCC_SOCKET_TRANSFER_FINISHED);
			return;
		default:
			break;
		}
		
	dccEvent();

	if (dccsock->check & GG_CHECK_WRITE)
		snw->setEnabled(true);

	if (dccevent) {
		gg_free_event(dccevent);
		dccevent = NULL;
		}

	in_watchDcc = false;
}

void dccSocketClass::askAccept(void) {
	QString str, f;
	QFileInfo fi;

	kdebug("dccSocketClass::askAccept()\n");
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
			kdebug("dccSocketClass::askAccept(): accepted\n");
			f = QFileDialog::getSaveFileName((char *)dccsock->file_info.filename,
				QString::null, 0, tr("save file"), tr("Select file location"));
			if (f.isEmpty()) {
				kdebug("dccSocketClass::askAccept(): discarded\n");
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
						kdebug("dccSocketClass::askAccept(): truncating file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) {
							QMessageBox::warning(kadu, tr("Connect error"), tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							return;
							}
						dccsock->offset = 0;
						break;
					case 1:
						kdebug("dccSocketClass::askAccept(): appending to file %s\n", f.latin1());

						if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_APPEND, 0600)) == -1) {
							QMessageBox::warning(kadu, tr("Connect error"), tr("Could not open file"));
							setState(DCC_SOCKET_COULDNT_OPEN_FILE);
							return;
							}
						dccsock->offset = fi.size();
						break;
					case 2:
						kdebug("dccSocketClass::askAccept(): discarded\n");
						setState(DCC_SOCKET_TRANSFER_DISCARDED);
						return;
					}
				}
			else {
				kdebug("dccSocketClass::askAccept(): creating file %s\n", f.latin1());

				if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT, 0600)) == -1) {
					QMessageBox::warning(kadu, tr("Connect error"), tr("Could not open file"));
					setState(DCC_SOCKET_COULDNT_OPEN_FILE);
					return;
					}
				dccsock->offset = 0;
				}

			filedialog = new DccFileDialog(this);
			filedialog->printFileInfo(dccsock);
			break;
		case 1:
			kdebug("dccSocketClass::askAccept(): discarded\n");
			setState(DCC_SOCKET_TRANSFER_DISCARDED);
			break;
		}
}


QString dccSocketClass::selectFile(void) {
	QString f;
	QFileInfo fi;
	do {
		f = QFileDialog::getOpenFileName((char *)dccsock->file_info.filename,
			QString::null, 0, tr("open file"), tr("Select file location"));
		fi.setFile(f);
	} while (f != QString::null && !fi.isReadable());

	return f;
}

void dccSocketClass::setState(int pstate) {
	snr->setEnabled(false);
	snw->setEnabled(false);
	state = pstate;
	if (filedialog)
		filedialog->dccFinished = true;
	dccSocketClass **me = new (dccSocketClass *);
	*me = this;
	qApp->postEvent((QObject *)kadu, new QCustomEvent(QEvent::User, me));
	kdebug("dccSocketClass::setState()\n");
}

void dccSocketClass::connectionBroken()
{
}

void dccSocketClass::dccError()
{
}

void dccSocketClass::dccEvent()
{
}

DccFileDialog::DccFileDialog(dccSocketClass *dccsocket, int type, QDialog *parent, const char *name)
	: QDialog (parent, name), dccsocket(dccsocket), type(type) {
	vlayout1 = new QVBoxLayout(this);
	vlayout1->setAutoAdd(true);
	vlayout1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	dccFinished = false;
}

DccFileDialog::~DccFileDialog() {
	delete time;
	if (!dccFinished) {
		kdebug("DccFileDialog::closeEvent(): DCC transfer has not finished yet!\n");
		delete dccsocket;
		}
}

void DccFileDialog::closeEvent(QCloseEvent *e) {
	QDialog::closeEvent(e);
}

void DccFileDialog::printFileInfo(struct gg_dcc *dccsock) {
	long long int percent;
 	long double fpercent;

	QLabel *l_sender = new QLabel(this);

	QString sender;

	if (type == DCC_TYPE_GET)
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
}

void DccFileDialog::updateFileInfo(struct gg_dcc *dccsock) {
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
}
