/***************************************************************************
                          expimp.cpp  -  description
                             -------------------
    begin                : Thu Feb 14 2002
    copyright            : (C) 2002 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <qlayout.h>
#include <kglobal.h>
#include <qpushbutton.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <stdlib.h>

#include "kadu.h"
#include "expimp.h"

UserlistImport::UserlistImport(QWidget *parent, const char *name)
 : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout * grid = new QGridLayout(this, 2, 2, 3, 3);

	results = new QListView(this);

	KIconLoader *loader = KGlobal::iconLoader();

	fetchbtn = new QPushButton(this);
	fetchbtn->setIconSet(QIconSet(loader->loadIcon("connect_creating", KIcon::Small) ));
	fetchbtn->setText(i18n("&Fetch userlist"));
	QObject::connect(fetchbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));

	QPushButton * savebtn = new QPushButton(this);
	savebtn->setIconSet(QIconSet(loader->loadIcon("filesave", KIcon::Small)));
	savebtn->setText(i18n("&Save results"));
	QObject::connect(savebtn, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	results->addColumn(i18n("UIN"));
	results->addColumn(i18n("Nickname"));
	results->addColumn(i18n("Disp. nick"));
	results->addColumn(i18n("Name"));
	results->addColumn(i18n("Surname"));
	results->addColumn(i18n("Mobile no."));
	results->addColumn(i18n("Group"));
	results->setAllColumnsShowFocus(true);

	grid->addMultiCellWidget(results, 0, 0, 0, 1);
	grid->addWidget(fetchbtn, 1, 0);
	grid->addWidget(savebtn, 1, 1);

	snr = snw = NULL;
	gg_http = NULL;
}

void UserlistImport::init() {
	resize(450, 330);
	setCaption(i18n("Import userlist"));	
}

void UserlistImport::deleteSocketNotifiers() {
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void UserlistImport::startTransfer() {
	if (!(gg_http = gg_userlist_get(config.uin, config.password, 1))) {
		fprintf(stderr, "KK UserlistImport: gg_userlist_get() failed\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered an internal error\nThe import was unsuccessful") );
		return;
		}

/*	int ret;
	while ((ret = gg_userlist_get_watch_fd(gg_http)) >=0 && gg_http->state != GG_STATE_CONNECTING);
	if (ret < 0) {
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}*/

	fetchbtn->setEnabled(false);

	snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void UserlistImport::closeEvent(QCloseEvent * e) {
	if (gg_http) {
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		}
	deleteSocketNotifiers();	
	QWidget::closeEvent(e);
}

void UserlistImport::updateUserlist() {
	int i;
	
	fprintf(stderr, "KK UserlistImport::updateUserlist()\n");
	
	i = 0;
	while (i < userlist.count()) {
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);
		i++;
		}

	userlist = importedUserlist;
//	importedUserlist.clear();
	
	kadu->userbox->clear();
	kadu->userbox->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	uin_t *uins;
	uins = (uin_t *) malloc(userlist.count() * sizeof(uin_t));

	for (i = 0; i < userlist.count(); i++)
		uins[i] = userlist[i].uin;

	gg_notify(sess, uins, userlist.count());
	fprintf(stderr, "KK UserlistImport::updateUserlist(): Userlist sent\n");

	userlist.writeToFile();
	fprintf(stderr, "KK UserlistImport::updateUserlist(): Wrote userlist\n");

	free(uins);			
}

void UserlistImport::dataReceived() {
	fprintf(stderr, "KK UserlistImport::dataReceived()\n");
	if (gg_http->check & GG_CHECK_READ)
		socketEvent();
}

void UserlistImport::dataSent() {
	fprintf(stderr, "KK UserlistImport::dataSent()\n");
	snw->setEnabled(false);
	if (gg_http->check & GG_CHECK_WRITE)
		socketEvent();
}

void UserlistImport::socketEvent() {
	bool dis2 = false;

	fprintf(stderr, "KK ImportUserlist::socketEvent(): data on socket\n");			

	if (gg_userlist_get_watch_fd(gg_http) < 0) {
		fetchbtn->setEnabled(true);
		fprintf(stderr,"KK ImportUserlist::socketEvent(): gg_userlist_get_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe import was unsuccessful") );
		deleteSocketNotifiers();
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_CONNECTING) {
		fprintf(stderr, "KK ImportUserlist::socketEvent(): changing QSocketNotifiers.\n");

		deleteSocketNotifiers();

		snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
		connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

		snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
		connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
		}

	if (gg_http->state == GG_STATE_DONE && gg_http->data == NULL) {
		fprintf(stderr, "KK ImportUserlist::socketEvent(): No results. Exit.\n");
		QMessageBox::information(this, "No results", i18n("Your action yielded no results") );
		fetchbtn->setEnabled(true);
		deleteSocketNotifiers();
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_ERROR) {
		fetchbtn->setEnabled(true);
		fprintf(stderr,"KK ImportUserlist::socketEvent(): gg_userlist_get_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe import was unsuccessful") );
		deleteSocketNotifiers();
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_DONE) {
		fetchbtn->setEnabled(true);
		fprintf(stderr,"KK ImportUserlist::socketEvent(): Done\n");
		QStringList strlist;
		cp_to_iso((unsigned char *)gg_http->data);
		strlist = QStringList::split("\r\n",__c2q((const char *)gg_http->data),true);
		fprintf(stderr, "! %d !\n", strlist.count());		
		fprintf(stderr, "%s\n", gg_http->data);
		QStringList fieldlist;
		QString tmparray[16];
		QListViewItem * qlv;
		int i, j;
		QStringList::Iterator it, it2;

		results->clear();
		importedUserlist.clear();
		for ((it = strlist.begin()), (i = 1); it != strlist.end(), i < strlist.count(); ++it, i++ ) {
			fieldlist = QStringList::split(";",*it,true);
			for ((it2 = fieldlist.begin()), (j = 1); it2 != fieldlist.end(), j < fieldlist.count(); ++it2, j++) {
				fprintf(stderr, "%s ",(const char *)(*it2).local8Bit());
				tmparray[j-1] = (*it2);
				}
			importedUserlist.addUser(tmparray[0], tmparray[1], tmparray[2],
				tmparray[3], tmparray[4], tmparray[6], GG_STATUS_NOT_AVAIL,
				tmparray[5]);

			if (tmparray[6] == "0")
				tmparray[6].truncate(0);	
			qlv = new QListViewItem(results, tmparray[6], tmparray[2], tmparray[3],
				tmparray[0], tmparray[1], tmparray[4], tmparray[5]);

			fprintf(stderr, "\n%s \n", (const char *)(*it).local8Bit());
			}

		deleteSocketNotifiers();
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->check & GG_CHECK_WRITE)
		snw->setEnabled(true);
}

UserlistExport::UserlistExport(QWidget *parent, const char *name)
 : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout *grid = new QGridLayout(this,3,1,3,3);

	KIconLoader *loader = KGlobal::iconLoader();

	QString message(i18n("%1 entries will be exported").arg(userlist.count()));

	QLabel *clabel = new QLabel(this);
	clabel->setText(message);

	sendbtn = new QPushButton(this);
	sendbtn->setText(i18n("&Send userlist"));
	sendbtn->setIconSet(QIconSet( loader->loadIcon("connect_creating", KIcon::Small)));

	QPushButton * closebtn = new QPushButton(this);
	closebtn->setText(i18n("&Close window"));
	closebtn->setIconSet(QIconSet(loader->loadIcon("stop", KIcon::Small)));

	QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	grid->addWidget(clabel,0,0);
	grid->addWidget(sendbtn,1,0);
	grid->addWidget(closebtn,2,0);

	QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	

	snw = snr = NULL;
	gg_http = NULL;
}

void UserlistExport::init() {
	setCaption(i18n("Export userlist"));	
}

void UserlistExport::deleteSocketNotifiers() {
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void UserlistExport::startTransfer() {
	QString contacts;
	int i = 0;
	
	while (i < userlist.count()) {
		contacts += userlist[i].first_name;
		contacts += ";";
		contacts += userlist[i].last_name;
		contacts += ";";
		contacts += userlist[i].nickname;
		contacts += ";";
		contacts += userlist[i].altnick;
		contacts += ";";
		contacts += userlist[i].mobile;
		contacts += ";";
		contacts += userlist[i].group;
		contacts += ";";
		contacts += QString::number(userlist[i].uin);
		contacts += ";\r\n";
		i++;
		}
	contacts.replace(QRegExp("(null)"), "");

	char *con2;
	con2 = strdup((const char *)contacts.local8Bit());
	iso_to_cp((unsigned char *)con2);
	
	if (!(gg_http = gg_userlist_put(config.uin, config.password, con2, 1))) {
		fprintf(stderr, "KK UserlistExport: gg_userlist_put() failed\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered an internal error\nThe export was unsuccessful") );
		return;
		}

/*	int ret;
	while ((ret = gg_userlist_put_watch_fd(gg_http)) >=0 && gg_http->state != GG_STATE_CONNECTING);
	if (ret < 0) {
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}*/

	sendbtn->setEnabled(false);

	snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void UserlistExport::dataReceived() {
	fprintf(stderr, "KK UserlistExport::dataReceived()\n");
	if (gg_http->check & GG_CHECK_READ)
		socketEvent();
}

void UserlistExport::dataSent() {
	fprintf(stderr, "KK UserlistExport::dataSent()\n");
	snw->setEnabled(false);
	if (gg_http->check & GG_CHECK_WRITE)
		socketEvent();
}

void UserlistExport::socketEvent() {
	fprintf(stderr, "KK ExportUserlist::socketEvent(): data on socket\n");			

	if (gg_userlist_put_watch_fd(gg_http) < 0) {
		sendbtn->setEnabled(true);
		fprintf(stderr,"KK ExportUserlist::socketEvent(): gg_userlist_put_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		deleteSocketNotifiers();
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_CONNECTING) {
		fprintf(stderr, "KK ExportUserlist::socketEvent(): changing QSocketNotifiers.\n");

		deleteSocketNotifiers();

		snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
		connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

		snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
		connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
		}

	if (gg_http->state == GG_STATE_ERROR) {
		sendbtn->setEnabled(true);
		fprintf(stderr,"KK ExportUserlist::socketEvent(): gg_userlist_put_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		deleteSocketNotifiers();
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_DONE) {
		sendbtn->setEnabled(true);
		fprintf(stderr,"KK Export Done\n");
		QMessageBox::information(this, "Export complete", i18n("Your userlist has been successfully exported to server") );
		deleteSocketNotifiers();				
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
        	}

	if (gg_http->check & GG_CHECK_WRITE)
		snw->setEnabled(true);
}

void UserlistExport::closeEvent(QCloseEvent * e) {
	deleteSocketNotifiers();
	if (gg_http) {
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		}
	QWidget::closeEvent(e);
}

#include "expimp.moc"
