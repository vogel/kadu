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

#include "kadu.h"
#include "expimp.h"

UserlistImport::UserlistImport(QDialog *parent, const char *name) : QDialog (parent, name) {
    resize(450, 330);
    setCaption(i18n("Import userlist"));	
    setWFlags(Qt::WDestructiveClose);

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

void UserlistImport::deleteSocketNotifiers() {
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
}

void UserlistImport::startTransfer() {
    if (!(gg_http = gg_userlist_get(config.uin, config.password, 1))) {
	fprintf(stderr, "KK UserlistImport: gg_userlist_get() failed\n");
	QMessageBox::critical(this, "Import error", i18n("The application encountered an internal error\nThe import was unsuccessful") );
	return;
	}

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
    int i, j, k;
    for (i = 0; i < importedusers.size(); i++) {
	j = 0;
	while (j < userlist.size() && importedusers[i].uin != userlist[j].uin)
	    j++;

	if (j >= userlist.size()) {
	    userlist.resize(userlist.size() + 1);
	    k = userlist.size() - 1;
    	    userlist[k].nickname = importedusers[i].nickname;
	    userlist[k].first_name = importedusers[i].first_name;
    	    userlist[k].last_name = importedusers[i].last_name;
    	    userlist[k].comment = importedusers[i].comment;
    	    userlist[k].group = importedusers[i].group;
    	    userlist[k].mobile = importedusers[i].mobile;
    	    userlist[k].description = NULL;
    	    userlist[k].uin = importedusers[i].uin;
	    }
        }

    kadu->syncUserlist();

    userlist_count = userlist.size();

    uin_t *uins;
    uins = (uin_t *) malloc(userlist.size() * sizeof(uin_t));
	
    for (i = 0; i < userlist.size(); i++)
	uins[i] = userlist[i].uin;

    gg_notify(&sess, uins, userlist_count);
    fprintf(stderr, "KK send_userlist(): Userlist sent\n");
    
    writeUserlist(NULL);
    fprintf(stderr, "KK Wrote userlist\n");
		
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
	fprintf(stderr,"Done\n");
	QStringList strlist;
	strlist = QStringList::split("\r\n",(char *)gg_http->data,true);
	fprintf(stderr, "! %d !\n", strlist.count());		
	fprintf(stderr, "%s\n", gg_http->data);
	QStringList fieldlist;
	QString tmparray[7];
	QListViewItem * qlv;
	int i, j;
	QStringList::Iterator it, it2;

        for ((it = strlist.begin()), (i = 1); it != strlist.end(), i < strlist.count(); ++it, i++ ) {
	    fieldlist = QStringList::split(";",*it,true);
	    for ((it2 = fieldlist.begin()), (j = 1); it2 != fieldlist.end(), j < fieldlist.count(); ++it2, j++) {
		fprintf(stderr, "%s ",(*it2).latin1());
		tmparray[j-1] = (*it2);
		}						

	    importedusers.resize(importedusers.size()+1);
	    importedusers[i-1].first_name = strdup(tmparray[0].local8Bit());
	    importedusers[i-1].last_name = strdup(tmparray[1].local8Bit());
	    importedusers[i-1].nickname = strdup(tmparray[2].local8Bit());
	    importedusers[i-1].comment = strdup(tmparray[3].local8Bit());
	    importedusers[i-1].mobile = strdup(tmparray[4].local8Bit());
	    importedusers[i-1].group = strdup(tmparray[5].local8Bit());
	    if (tmparray[6])
		importedusers[i-1].uin = tmparray[6].toInt();
	    else
		importedusers[i-1].uin = 0;

	    importedusers[i-1].status = GG_STATUS_NOT_AVAIL;

    	    qlv = new QListViewItem(results, tmparray[6], __c2q(tmparray[2]), __c2q(tmparray[3]), __c2q(tmparray[0]), __c2q(tmparray[1]), tmparray[4], __c2q(tmparray[5]));

            fprintf(stderr, "\n%s \n", (*it).latin1());
	    }

	deleteSocketNotifiers();
	gg_userlist_get_free(gg_http);
	gg_http = NULL;

	return;
	}
    if (gg_http->check & GG_CHECK_WRITE)
	snw->setEnabled(true);
}

UserlistExport::UserlistExport(QDialog *parent, const char *name) : QDialog (parent, name) {
//	this->resize(450,330);
    setCaption(i18n("Export userlist"));	
    setWFlags(Qt::WDestructiveClose);

    QGridLayout * grid = new QGridLayout(this,3,1,3,3);

    KIconLoader *loader = KGlobal::iconLoader();

    QString message(i18n("%1 entries will be exported").arg(userlist.size()));

    QLabel * clabel = new QLabel(this);
    clabel->setText(message);

    sendbtn = new QPushButton(this);
    sendbtn->setText(i18n("&Send userlist"));
    sendbtn->setIconSet(QIconSet( loader->loadIcon("connect_creating", KIcon::Small)));

    QPushButton * closebtn = new QPushButton(this);
    closebtn->setText(i18n("&Close window"));
    closebtn->setIconSet(QIconSet( loader->loadIcon("stop", KIcon::Small)));

    QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

    grid->addWidget(clabel,0,0);
    grid->addWidget(sendbtn,1,0);
    grid->addWidget(closebtn,2,0);

    QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	
    
    snw = snr = NULL;
    gg_http = NULL;
}

void UserlistExport::deleteSocketNotifiers() {
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
}

void UserlistExport::startTransfer() {
    QString contacts;
    int i = 0;
    char t_uin[12];
    while (i < userlist.size()) {
	contacts += __c2q(userlist[i].first_name);
	contacts += __c2q(";");
	contacts += __c2q(userlist[i].last_name);
	contacts += __c2q(";");
	contacts += __c2q(userlist[i].nickname);
	contacts += __c2q(";");
	contacts += __c2q(userlist[i].comment);
	contacts += __c2q(";");
	contacts += __c2q(userlist[i].mobile);
	contacts += __c2q(";");
	contacts += __c2q(userlist[i].group);
	contacts += __c2q(";");
	snprintf(t_uin,sizeof(t_uin),"%d",userlist[i].uin);
	contacts += __c2q(t_uin);
	contacts += __c2q(";\r\n");
	i++;
	}
    contacts.replace( QRegExp("(null)"), "" );

    char * con2;
    con2 = strdup(contacts.local8Bit());

    if (!(gg_http = gg_userlist_put(config.uin, config.password, con2, 1))) {
	fprintf(stderr, "KK UserlistExport: gg_userlist_put() failed\n");
	QMessageBox::critical(this, "Export error", i18n("The application encountered an internal error\nThe export was unsuccessful") );
	return;
	}

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
