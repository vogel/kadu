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

#include "gadu.h"
#include "kadu.h"
#include "config_dialog.h"
#include "debug.h"
#include "events.h"
#include "message_box.h"
#include "ignore.h"
#include "expimp.h"
#include "status.h"

UserlistImport::UserlistImport(QWidget *parent, const char *name)
 : QDialog(parent, name, FALSE, Qt::WDestructiveClose) {
	results = new QListView(this);

	fetchbtn = new QPushButton(QIconSet(icons_manager.loadIcon("FetchUserlist")),tr("&Fetch userlist"), this);
	QObject::connect(fetchbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));

	QPushButton *filebtn = new QPushButton(QIconSet(icons_manager.loadIcon("ImportFromFile")),tr("&Import from file"), this);
	QObject::connect(filebtn, SIGNAL(clicked()), this, SLOT(fromfile()));

	QPushButton *savebtn = new QPushButton(QIconSet(icons_manager.loadIcon("SaveUserlist")),tr("&Save results"), this);
	QObject::connect(savebtn, SIGNAL(clicked()), this, SLOT(makeUserlist()));

	QPushButton *mergebtn = new QPushButton(QIconSet(icons_manager.loadIcon("MergeUserlist")),tr("&Merge results"), this);
	QObject::connect(mergebtn, SIGNAL(clicked()), this, SLOT(updateUserlist()));

	results->addColumn(tr("UIN"));
	results->addColumn(tr("Nickname"));
	results->addColumn(tr("Disp. nick"));
	results->addColumn(tr("Name"));
	results->addColumn(tr("Surname"));
	results->addColumn(tr("Mobile no."));
	results->addColumn(tr("Group"));
	results->addColumn(tr("Email"));
	results->setAllColumnsShowFocus(true);

	QGridLayout * grid = new QGridLayout(this, 2, 3, 3, 3);
	grid->addMultiCellWidget(results, 0, 0, 0, 3);
	grid->addWidget(filebtn, 1, 0);
	grid->addWidget(fetchbtn, 1, 1);
	grid->addWidget(savebtn, 1, 2);
	grid->addWidget(mergebtn, 1, 3);

	resize(450, 330);
	setCaption(tr("Import userlist"));

	connect(gadu, SIGNAL(userListImported(bool, UserList&)), this, SLOT(userListImported(bool, UserList&)));
}

void UserlistImport::fromfile() {
	kdebugf();
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (fname.length()) {
		QFile file(fname);
 		if (file.open(IO_ReadOnly)) {
			QTextStream stream(&file);
			gadu->streamToUserList(stream, importedUserlist);
			file.close();
			}
		else
			QMessageBox::critical(this, tr("Import error"),
				tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
		}
}

void UserlistImport::startTransfer() {
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	if (gadu->doImportUserList())
		fetchbtn->setEnabled(false);
}

void UserlistImport::makeUserlist() {
	unsigned int i;
	
	kdebugf();
	
	for (i=0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);

	userlist = importedUserlist;
	
	clearIgnored();
	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox()->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	userlist.writeToFile();
	kdebug("UserlistImport::makeUserlist(): Wrote userlist\n");
}

void UserlistImport::updateUserlist() {
	unsigned int i;
	
	kdebugf();
	
	for (i=0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_remove_notify(sess, userlist[i].uin);

	userlist.merge(importedUserlist);
	
	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();
	for (i = 0; i < userlist.count(); i++)
		kadu->userbox()->addUser(userlist[i].altnick);
		
	UserBox::all_refresh();

	for (i = 0; i < userlist.count(); i++)
		if (userlist[i].uin)
			gg_add_notify(sess, userlist[i].uin);

	userlist.writeToFile();
	kdebug("UserlistImport::updateUserlist(): Wrote userlist\n");
}

void UserlistImport::userListImported(bool ok, UserList& userList)
{
	kdebug("UserlistImport::userListImported()\n");

	importedUserlist = userList;
	results->clear();

	fetchbtn->setEnabled(true);

	if (ok)
		for (unsigned int i = 0; i < userList.count(); i++)
			new QListViewItem(results, QString::number(userList[i].uin), userList[i].nickname, userList[i].altnick, userList[i].first_name,
				userList[i].last_name, userList[i].mobile, userList[i].group(), userList[i].email);
}

UserlistExport::UserlistExport(QWidget *parent, const char *name)
 : QDialog (parent, name, FALSE, Qt::WDestructiveClose) {

	QGridLayout *grid = new QGridLayout(this,3,1,3,3);

	int i = 0;
	for (UserList::iterator it = userlist.begin(); it != userlist.end(); it++)
		if (!(*it).anonymous)
			i++;

	QString message(tr("%1 entries will be exported").arg(userlist.count()));

	QLabel *clabel = new QLabel(message,this);

	sendbtn = new QPushButton(QIconSet(icons_manager.loadIcon("SendUserlist")),tr("&Send userlist"),this);
	
	deletebtn = new QPushButton(QIconSet(icons_manager.loadIcon("DeleteUserlist")),tr("&Delete userlist"),this);
	
	tofilebtn = new QPushButton(QIconSet(icons_manager.loadIcon("ExportUserlist")),tr("&Export to file"),this);

	QPushButton * closebtn = new QPushButton(QIconSet(icons_manager.loadIcon("CloseWindow")),tr("&Close window"),this);

	QObject::connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	grid->addWidget(clabel,0,0);
	grid->addWidget(sendbtn,1,0);
	grid->addWidget(deletebtn,2,0);	
	grid->addWidget(tofilebtn,3,0);		
	grid->addWidget(closebtn,4,0);

	QObject::connect(sendbtn, SIGNAL(clicked()), this, SLOT(startTransfer()));	
	QObject::connect(tofilebtn, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	QObject::connect(deletebtn, SIGNAL(clicked()), this, SLOT(clean()));

	connect(gadu, SIGNAL(userListExported(bool)), this, SLOT(userListExported(bool)));
	connect(gadu, SIGNAL(userListCleared(bool)), this, SLOT(userListCleared(bool)));

	setCaption(tr("Export userlist"));	
}

void UserlistExport::startTransfer()
{
	kdebugf();

	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	if (gadu->doExportUserList(userlist))
	{
		sendbtn->setEnabled(false);
		deletebtn->setEnabled(false);
		tofilebtn->setEnabled(false);	
	}
}

void UserlistExport::ExportToFile(void)
{
	kdebugf();
	QString contacts;
	sendbtn->setEnabled(false);
	deletebtn->setEnabled(false);
	tofilebtn->setEnabled(false);	

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (fname.length()) {
		contacts = gadu->userListToString(userlist);

		QFile file(fname);
		if (file.open(IO_WriteOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			stream << contacts;
			file.close();
			QMessageBox::information(this, tr("Export completed"),
				tr("Your userlist has been successfully exported to file"));
			}
		else
			QMessageBox::critical(this, tr("Export error"),
				tr("The application encountered an internal error\nThe export userlist to file was unsuccessful"));
		}

	sendbtn->setEnabled(true);
	deletebtn->setEnabled(true);
	tofilebtn->setEnabled(true);
}

void UserlistExport::clean() {
	kdebugf();

	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
		return;

	if (gadu->doClearUserList())
	{
		deletebtn->setEnabled(false);
		sendbtn->setEnabled(false);
		tofilebtn->setEnabled(false);
	}
}

void UserlistExport::userListExported(bool ok)
{
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully exported to server"));
	else
		QMessageBox::critical(this, tr("Export error"),
			tr("The application encountered an internal error\nThe export was unsuccessful"));

	sendbtn->setEnabled(true);
	tofilebtn->setEnabled(true);
	deletebtn->setEnabled(true);
}

void UserlistExport::userListCleared(bool ok)
{
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully deleted on server"));
	else
		QMessageBox::critical(this, tr("Export error"),
			tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"));

	sendbtn->setEnabled(true);
	tofilebtn->setEnabled(true);
	deletebtn->setEnabled(true);
}

