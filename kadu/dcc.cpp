/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <iostream>
#include <fcntl.h>
#include <klocale.h>
#include <qmessagebox.h>
#include <unistd.h>
#include <qprocess.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
//
#include "kadu.h"
#include "dcc.h"
//

dccSocketClass::dccSocketClass(struct gg_dcc *dcc_sock) : QObject() {
	dccsock = dcc_sock;
	dccevent = NULL;
	snr = snw = NULL;
	state = DCC_SOCKET_TRANSFERRING;
	dialog = NULL;
	recordprocess = playprocess = NULL;
	in_watchDcc = false;
}

dccSocketClass::~dccSocketClass() {
	fprintf(stderr, "KK dccSocketClass::~dccSocketClass\n");
	if (dialog) {
		if (dialog->dccFinished) {
			if (dialog->isVisible())
				dialog->close();
			else
				delete dialog;    
			dialog = NULL;
			}
		}
	if (recordprocess)
		{
		recordprocess->kill();
		delete recordprocess;
		recordprocess = NULL;
		}
	if (playprocess)
		{
		playprocess->kill();
		delete playprocess;
		playprocess = NULL;
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
		}
}

void dccSocketClass::initializeNotifiers() {
	snr = new QSocketNotifier(dccsock->fd, QSocketNotifier::Read, this);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dccDataReceived()));

	snw = new QSocketNotifier(dccsock->fd, QSocketNotifier::Write, this);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dccDataSent()));
}

void dccSocketClass::dccDataReceived() {
	if (!in_watchDcc) {
		fprintf(stderr, "KK dccSocketClass::dccDataReceived\n");
		watchDcc(GG_CHECK_READ);
		}
}

void dccSocketClass::dccDataSent() {
	fprintf(stderr, "KK dccSocketClass::dccDataSent\n");
	snw->setEnabled(false);
	if (dccsock->check & GG_CHECK_WRITE)
		watchDcc(GG_CHECK_WRITE);
}

void dccSocketClass::watchDcc(int check) {
	QString f;
	struct sockaddr_un addr;
	int sock;
	int len;
	char buf[195];

	in_watchDcc = true;

	fprintf(stderr, "KK dccSocketClass::watchDcc()\n");			
	if (!(dccevent = gg_dcc_watch_fd(dccsock))) {
		fprintf(stderr, "KK dccSocketClass::watchDcc: Connection broken unexpectedly!\n");
		setState(DCC_SOCKET_CONNECTION_BROKEN);
    		return;
		}

	switch (dccevent->type) {
		case GG_EVENT_NONE:	    
			if (dialog && dialog->isVisible())
				dialog->updateFileInfo(dccsock);
			break;
		case GG_EVENT_DCC_CALLBACK:
			gg_dcc_set_type(dccsock, GG_SESSION_DCC_SEND);
			break;
		case GG_EVENT_DCC_NEED_FILE_ACK:
			fprintf(stderr,"KK dccSocketClass::watchDcc:  GG_EVENT_DCC_NEED_FILE_ACK! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			askAccept();
			break;
		case GG_EVENT_DCC_NEED_FILE_INFO:
			fprintf(stderr,"KK dccSocketClass::watchDcc:  GG_EVENT_DCC_NEED_FILE_INFO! %d %d\n",
				dccsock->uin, dccsock->peer_uin);
			f = selectFile();
			if (f == NULL) {
				fprintf(stderr, "KK dccSocketClass::watchDcc: Abort transfer\n");
				setState(DCC_SOCKET_TRANSFER_ERROR);
				return;
				}
			gg_dcc_fill_file_info(dccsock, f.local8Bit());
			dialog = new DccGet(this, DCC_TYPE_SEND);
			dialog->printFileInfo(dccsock);
			break;	    
		case GG_EVENT_DCC_NEED_VOICE_ACK:
			unlink("/home/chilek/.gg/kaduplayvoice");			
			playprocess = new QProcess(QString("kaduplayvoice"));
			playprocess->start();
			fprintf(stderr, "KK accept playsocket\n");
			sock = socket(PF_UNIX, SOCK_STREAM, 0);
			addr.sun_family = AF_UNIX;
			strcpy(addr.sun_path, "/home/chilek/.gg/kaduplayvoice");
			bind(sock, (const sockaddr *)&addr, sizeof(addr));
			listen(sock, 5);
			playsocket = accept(sock, NULL, 0);

			unlink("/home/chilek/.gg/kadurecordvoice");			
			recordprocess = new QProcess(QString("kadurecordvoice"));
			recordprocess->start();
			fprintf(stderr, "KK accept recordsocket\n");
			sock = socket(PF_UNIX, SOCK_STREAM, 0);
			addr.sun_family = AF_UNIX;
			strcpy(addr.sun_path, "/home/chilek/.gg/kadurecordvoice");
			bind(sock, (const sockaddr *)&addr, sizeof(addr));
			listen(sock, 5);
			recordsocket = accept(sock, NULL, 0);
			break;
		case GG_EVENT_DCC_VOICE_DATA:
			if (!playprocess->isRunning())
				break;
			write(playsocket, &dccevent->event.dcc_voice_data.length, sizeof(int));
			write(playsocket, dccevent->event.dcc_voice_data.data,
				dccevent->event.dcc_voice_data.length);
			read(recordsocket, &len, sizeof(int));
			read(recordsocket, buf, len);
			gg_dcc_voice_send(dccsock, buf, len);
			break;
		case GG_EVENT_DCC_ERROR:
			fprintf(stderr, "KK dccSocketClass::watchDcc: GG_EVENT_DCC_ERROR\n");
			setState(DCC_SOCKET_TRANSFER_ERROR);
			return;
		case GG_EVENT_DCC_DONE:
			fprintf(stderr, "KK dccSocketClass::watchDcc: GG_EVENT_DCC_DONE\n");
			if (dialog && dialog->isVisible())
				dialog->updateFileInfo(dccsock);
			setState(DCC_SOCKET_TRANSFER_FINISHED);
			return;
		default:
			break;
		}

	if (dccsock->check & GG_CHECK_WRITE)
		snw->setEnabled(true);

	if (dccevent) {
		gg_free_event(dccevent);
		dccevent = NULL;
		}

	in_watchDcc = false;
}

void dccSocketClass::askAccept(void) {
	QString str,f;

	fprintf(stderr,"KK dccSocketClass::askAccept\n");
	str.append(i18n("User "));
	str.append(userlist.byUin(dccsock->peer_uin).altnick);
	str.append(i18n(" wants to send us a file "));
	str.append((char *)dccsock->file_info.filename);
	str.append(i18n("\nof size "));

	char fsize[20];
	snprintf(fsize, sizeof(fsize), "%.1fKB", (float) dccsock->file_info.size/1024);
	str.append(fsize);
	str.append(i18n(". Accept transfer?"));

	switch (QMessageBox::information( 0, i18n("Incoming transfer"),str, i18n("Yes"), i18n("No"),
		QString::null, 0, 1) ) {
		case 0: // Yes?
			fprintf(stderr, "KK dccSocketClass::askAccept: accepted\n");
			f = QFileDialog::getSaveFileName((char *)dccsock->file_info.filename,
				QString::null, 0, i18n("save file"), i18n("Select file location"));
			if (f.isEmpty()) {
				fprintf(stderr, "KK dccSocketClass::askAccept: discarded\n");
				setState(DCC_SOCKET_TRANSFER_DISCARDED);
				break;
				}

			fprintf(stderr, "KK dccSocketClass::askAccept: opening file %s\n", f.latin1());

			if ((dccsock->file_fd = open(f.latin1(), O_WRONLY | O_CREAT, 0600)) == -1) {
				QMessageBox::warning(kadu, i18n("Connect error"), i18n("Could not open file"));
				setState(DCC_SOCKET_COULDNT_OPEN_FILE);
				}

			dialog = new DccGet(this);
			dialog->printFileInfo(dccsock);
			break;
		case 1:
			fprintf(stderr, "KK dccSocketClass::askAccept: discarded\n");
			setState(DCC_SOCKET_TRANSFER_DISCARDED);
			break;
		}
}

QString dccSocketClass::selectFile(void) {
	QString f;

	f = QFileDialog::getOpenFileName((char *)dccsock->file_info.filename,
		QString::null, 0, i18n("open file"), i18n("Select file location"));
	
	return f;
}

void dccSocketClass::setState(int pstate) {
	snr->setEnabled(false);
	snw->setEnabled(false);
	state = pstate;
	if (dialog)
		dialog->dccFinished = true;
	dccSocketClass **me = new (dccSocketClass *);
	*me = this;
	a->postEvent((QObject *)kadu, new QCustomEvent(QEvent::User, me));
	fprintf(stderr, "KK dccSocketClass::setState\n");
}

DccGet::DccGet(dccSocketClass *dccsocket, int type, QDialog *parent, const char *name)
	: QDialog (parent, name), type(type), dccsocket(dccsocket) {
	vbox1 = new QVBox(this);
	vbox1->setMargin(5);
	setWFlags(Qt::WDestructiveClose);
	prevPercent = 0;
	prevOffset = 0;
	dccFinished = false;
}

DccGet::~DccGet() {
	delete time;
}

void DccGet::closeEvent(QCloseEvent *e) {
	QWidget::closeEvent(e);
	if (!dccFinished) {
		fprintf(stderr, "KK DccGet::closeEvent: DCC transfer has not finished yet!\n");
		delete dccsocket;
		}
}

void DccGet::printFileInfo(struct gg_dcc *dccsock) {

	QLabel *l_sender = new QLabel(vbox1);

	QString sender;

	if (type == DCC_TYPE_GET)
		sender.append(i18n("Sender: "));
	else
		sender.append(i18n("Receiver: "));
	sender.append(userlist.byUin(dccsock->peer_uin).altnick);
	l_sender->setText(sender);

	QLabel *l_filename = new QLabel(vbox1);
	sender.truncate(0);

	sender.append(i18n("Filename: "));
	sender.append((char *)dccsock->file_info.filename);
	l_filename->setText(sender);

	QLabel *l_filesize = new QLabel(vbox1);
	sender.truncate(0);

	sender.append(i18n("File size: "));
	sender.append(QString::number(dccsock->file_info.size));
	sender.append("B");

	l_filesize->setText(sender);

	l_offset = new QLabel(vbox1);			
	l_offset->setText(i18n("Speed: 0KB/s (not started)  "));

	p_progress = new QProgressBar(100, vbox1);
	p_progress->setProgress(0);

	time = new QTime();
	time->start();
	prevOffset = dccsock->offset;

	vbox1->resize(vbox1->sizeHint());
	resize(vbox1->sizeHint().width() + 15, vbox1->sizeHint().height() + 15);

	setCaption(i18n("File transfer"));
	show();
}

void DccGet::updateFileInfo(struct gg_dcc *dccsock) {
	long long int percent;
 	long double fpercent;
	int diffOffset,diffTime;
	QString str;

	if ((diffTime = time->elapsed()) > 1000) {
		diffOffset = dccsock->offset - prevOffset;
		prevOffset = dccsock->offset;
		str.append(i18n("Speed: "));
		str.append(QString::number(diffOffset/1024));
		str.append("KB/s ");
		if (!diffOffset)
			str.append(i18n("(stalled)"));
		l_offset->setText(str);
		time->restart();	
		}
	fpercent = ((long double)dccsock->offset * 100.0) / (long double)dccsock->file_info.size;
	percent = (long long int) fpercent;
	if (percent > prevPercent) {
		p_progress->setProgress(percent);
		prevPercent = percent;
		}
}

#include "dcc.moc"
