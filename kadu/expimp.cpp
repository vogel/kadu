/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <stdlib.h>

#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "expimp.h"

UserlistImport::UserlistImport(QWidget *parent, const char *name)
 : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout * grid = new QGridLayout(this, 2, 2, 3, 3);

	results = new QListView(this);

	fetchbtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Fetch userlist"),this);
	QObject::connect(fetchbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));

	QPushButton * savebtn = new QPushButton(QIconSet(loadIcon("filesave.png")),i18n("&Save results"),this);
	QObject::connect(savebtn, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	QPushButton * filebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Import from file"),this);
	QObject::connect(filebtn, SIGNAL(clicked()), this, SLOT(fromfile()));


	results->addColumn(i18n("UIN"));
	results->addColumn(i18n("Nickname"));
	results->addColumn(i18n("Disp. nick"));
	results->addColumn(i18n("Name"));
	results->addColumn(i18n("Surname"));
	results->addColumn(i18n("Mobile no."));
	results->addColumn(i18n("Group"));
	results->setAllColumnsShowFocus(true);

	grid->addMultiCellWidget(results, 0, 0, 0, 2);
	grid->addWidget(filebtn, 1, 0);	
	grid->addWidget(fetchbtn, 1, 1);
	grid->addWidget(savebtn, 1, 2);

	snr = snw = NULL;
	gg_http = NULL;
}


void UserlistImport::fromfile(){

	QStringList lines,userlist;
	QListViewItem * qlv;
	QString line;    
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (fname.length()){
		QFile file(fname);
 		if (file.open(IO_ReadOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			importedUserlist.clear();
			results->clear();
			while ( !stream.eof() ) {
				line = stream.readLine();
				lines = QStringList::split(";",line,true);
				if (lines[6] == "0")
					lines[6].truncate(0);
				importedUserlist.addUser(lines[0], lines[1], lines[2],
					lines[3], lines[4], lines[6], GG_STATUS_NOT_AVAIL,
					false, false, true, lines[5]);
				qlv = new QListViewItem(results, lines[6], lines[2], lines[3],
					lines[0], lines[1], lines[4], lines[5]);				
	  			}
			file.close();
			}
		else
			QMessageBox::critical(this, i18n("Import error"),
				i18n("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
		}
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
		kdebug("UserlistImport: gg_userlist_get() failed\n");
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
	
	kdebug("UserlistImport::updateUserlist()\n");
	
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
	kdebug("UserlistImport::updateUserlist(): Userlist sent\n");

	userlist.writeToFile();
	kdebug("UserlistImport::updateUserlist(): Wrote userlist\n");

	free(uins);			
}

void UserlistImport::dataReceived() {
	kdebug("UserlistImport::dataReceived()\n");
	if (gg_http->check & GG_CHECK_READ)
		socketEvent();
}

void UserlistImport::dataSent() {
	kdebug("UserlistImport::dataSent()\n");
	snw->setEnabled(false);
	if (gg_http->check & GG_CHECK_WRITE)
		socketEvent();
}

void UserlistImport::socketEvent() {
//	bool dis2 = false;

	kdebug("ImportUserlist::socketEvent(): data on socket\n");			

	if (gg_userlist_get_watch_fd(gg_http) < 0) {
		fetchbtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ImportUserlist::socketEvent(): gg_userlist_get_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe import was unsuccessful") );
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_CONNECTING) {
		kdebug("ImportUserlist::socketEvent(): changing QSocketNotifiers.\n");		
		deleteSocketNotifiers();
		snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
		connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
		snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
		connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
		}

	if (gg_http->state == GG_STATE_DONE && gg_http->data == NULL) {
		kdebug("ImportUserlist::socketEvent(): No results. Exit.\n");
		deleteSocketNotifiers();
		QMessageBox::information(this, "No results", i18n("Your action yielded no results") );
		fetchbtn->setEnabled(true);
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_ERROR) {
		fetchbtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ImportUserlist::socketEvent(): gg_userlist_get_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Import error", i18n("The application encountered a network error\nThe import was unsuccessful") );
		gg_userlist_get_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_DONE) {
		fetchbtn->setEnabled(true);
		kdebug("ImportUserlist::socketEvent(): Done\n");
		QStringList strlist;
		cp2unicode((unsigned char *)gg_http->data);
		strlist = QStringList::split("\r\n", cp2unicode((unsigned char *)gg_http->data), true);
		kdebug("! %d !\n", strlist.count());		
		kdebug("%s\n", gg_http->data);
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
				kdebug("%s ",(const char *)(*it2).local8Bit());
				tmparray[j-1] = (*it2);
				}
			importedUserlist.addUser(tmparray[0], tmparray[1], tmparray[2],
				tmparray[3], tmparray[4], tmparray[6], GG_STATUS_NOT_AVAIL,
				false, false, true, tmparray[5]);

			if (tmparray[6] == "0")
				tmparray[6].truncate(0);	
			qlv = new QListViewItem(results, tmparray[6], tmparray[2], tmparray[3],
				tmparray[0], tmparray[1], tmparray[4], tmparray[5]);

			kdebug("\n%s \n", (const char *)(*it).local8Bit());
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

	QString message(i18n("%1 entries will be exported").arg(userlist.count()));

	QLabel *clabel = new QLabel(message,this);

	sendbtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Send userlist"),this);
	
	deletebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Delete userlist"),this);
	
	tofilebtn = new QPushButton(QIconSet(loadIcon("connect_creating.png")),i18n("&Export to file"),this);

	QPushButton * closebtn = new QPushButton(QIconSet(loadIcon("stop.png")),i18n("&Close window"),this);

	QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	grid->addWidget(clabel,0,0);
	grid->addWidget(sendbtn,1,0);
	grid->addWidget(deletebtn,2,0);	
	grid->addWidget(tofilebtn,3,0);		
	grid->addWidget(closebtn,4,0);

	QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	
	QObject::connect(tofilebtn, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	QObject::connect(deletebtn, SIGNAL(clicked()), this, SLOT(clean()));		

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

QString UserlistExport::saveContacts(){
	QString contacts;
	int i = 0;
	contacts="";
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
	
	return contacts;
}

void UserlistExport::startTransfer() {
	QString contacts;
	contacts=saveContacts();
	
	char *con2;	
	con2 = (char *)strdup(unicode2cp(contacts).data());
	
	if (!(gg_http = gg_userlist_put(config.uin, config.password, con2, 1))) {
		kdebug("UserlistExport: gg_userlist_put() failed\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered an internal error\nThe export was unsuccessful") );
		free(con2);
		return;
		}
	free(con2);

/*	int ret;
	while ((ret = gg_userlist_put_watch_fd(gg_http)) >=0 && gg_http->state != GG_STATE_CONNECTING);
	if (ret < 0) {
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}*/

	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	
	snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void UserlistExport::ExportToFile(void) {
	QString contacts;
	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (fname.length()) {
		contacts = saveContacts();

		QFile file(fname);
		if (file.open(IO_WriteOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			stream << contacts;
			file.close();
			QMessageBox::information(this, "Export completed", i18n("Your userlist has been successfully exported to file"));
			}
		else
			QMessageBox::critical(this, "Export error", i18n("The application encountered an internal error\nThe export userlist to file was unsuccessful") );
		}

	sendbtn->setEnabled(true);
	deletebtn->setEnabled(true);
	tofilebtn->setEnabled(true);
}

void UserlistExport::clean() {
	const char *con2="";
	deletebtn->setEnabled(false);
	sendbtn->setEnabled(false);
	tofilebtn->setEnabled(false);
	if (!(gg_http = gg_userlist_put(config.uin, config.password, con2, 1))) {
		kdebug("UserlistExport: Delete failed\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered an internal error\nThe delete userlist on server was unsuccessful") );
		deletebtn->setEnabled(true);		
		sendbtn->setEnabled(true);
		tofilebtn->setEnabled(true);
		return;
    		}
			
	snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

	snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
				
}

void UserlistExport::dataReceived() {
	kdebug("UserlistExport::dataReceived()\n");
	if (gg_http->check & GG_CHECK_READ)
		socketEvent();
}

void UserlistExport::dataSent() {
	kdebug("UserlistExport::dataSent()\n");
	snw->setEnabled(false);
	if (gg_http->check & GG_CHECK_WRITE)
		socketEvent();
}

void UserlistExport::socketEvent() {
	kdebug("ExportUserlist::socketEvent(): data on socket\n");			

	if (gg_userlist_put_watch_fd(gg_http) < 0) {
		deletebtn->setEnabled(true);
		sendbtn->setEnabled(true);
		tofilebtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ExportUserlist::socketEvent(): gg_userlist_put_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_CONNECTING) {
		kdebug("ExportUserlist::socketEvent(): changing QSocketNotifiers.\n");
		deleteSocketNotifiers();
		snr = new QSocketNotifier(gg_http->fd, QSocketNotifier::Read, this);
		connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));

		snw = new QSocketNotifier(gg_http->fd, QSocketNotifier::Write, this);
		connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
		}

	if (gg_http->state == GG_STATE_ERROR) {
		sendbtn->setEnabled(true);
		deletebtn->setEnabled(true);
		tofilebtn->setEnabled(true);
		deleteSocketNotifiers();
		kdebug("ExportUserlist::socketEvent(): gg_userlist_put_watch_fd() error. bailing out.\n");
		QMessageBox::critical(this, "Export error", i18n("The application encountered a network error\nThe export was unsuccessful") );
		gg_userlist_put_free(gg_http);
		gg_http = NULL;
		return;
		}

	if (gg_http->state == GG_STATE_DONE) {
		sendbtn->setEnabled(true);
		tofilebtn->setEnabled(true);
		deletebtn->setEnabled(true);
		kdebug("Export Done\n");
		deleteSocketNotifiers();
		QMessageBox::information(this, "Export complete", i18n("Your userlist has been successfully exported to server") );
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

