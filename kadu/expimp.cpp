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
#include <qregexp.h>
#include <qtextcodec.h>
#include <stdlib.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "gadu.h"
#include "kadu.h"
#include "debug.h"
#include "message_box.h"
#include "ignore.h"
#include "expimp.h"
#include "status.h"

UserlistImportExport::UserlistImportExport(QWidget *parent, const char *name) : QHBox(parent, name, WType_TopLevel|WDestructiveClose)
{
	kdebugf();
	setCaption(tr("Import / export userlist"));
	
	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left);
	QWidget *w_blank = new QWidget(left);
	w_blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("ImportExportWindowIcon"));
	l_info->setText(tr("This dialog box allows you to import and export your buddy list to a server or a file."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	// our QListView
	// our QVGroupBox
	QVGroupBox *vgb_import = new QVGroupBox(center);
	vgb_import->setTitle(tr("Import userlist"));
	// end our QGroupBox
	lv_userlist = new QListView(vgb_import);
	lv_userlist->addColumn(tr("UIN"));
	lv_userlist->addColumn(tr("Nickname"));
	lv_userlist->addColumn(tr("Disp. nick"));
	lv_userlist->addColumn(tr("Name"));
	lv_userlist->addColumn(tr("Surname"));
	lv_userlist->addColumn(tr("Mobile no."));
	lv_userlist->addColumn(tr("Group"));
	lv_userlist->addColumn(tr("Email"));
	lv_userlist->setAllColumnsShowFocus(true);
	// end our QListView

	// buttons
	QHBox *hb_importbuttons = new QHBox(vgb_import);
	QWidget *w_blank2 = new QWidget(hb_importbuttons);
	hb_importbuttons->setSpacing(5);
	w_blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	
	pb_fetch = new QPushButton(icons_manager.loadIcon("FetchUserList"), tr("&Fetch userlist"), hb_importbuttons, "fetch");
	QPushButton *pb_file = new QPushButton(icons_manager.loadIcon("ImportFromFile"), tr("&Import from file"), hb_importbuttons, "file");
	QPushButton *pb_save = new QPushButton(icons_manager.loadIcon("SaveUserlist"), tr("&Save results"), hb_importbuttons, "save");
	QPushButton *pb_merge = new QPushButton(icons_manager.loadIcon("MergeUserlist"), tr("&Merge results"), hb_importbuttons, "merge");
	// end buttons
	
	// our QVGroupBox
	QVGroupBox *vgb_export = new QVGroupBox(center);
	vgb_export->setTitle(tr("Export userlist"));
	// end our QGroupBox

	l_itemscount = new QLabel(vgb_export);
	l_itemscount->setText(tr("%1 entries will be exported").arg(userlist.count()));
	
	// export buttons
	QHBox *hb_exportbuttons = new QHBox(vgb_export);
	QWidget *w_blank3 = new QWidget(hb_exportbuttons);
	hb_exportbuttons->setSpacing(5);
	w_blank3->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	
	pb_send = new QPushButton(icons_manager.loadIcon("SendUserlist"),tr("Se&nd userlist"), hb_exportbuttons, "send");
	pb_delete = new QPushButton(icons_manager.loadIcon("DeleteUserlist"),tr("&Delete userlist"), hb_exportbuttons, "delete");
	pb_tofile = new QPushButton(icons_manager.loadIcon("ExportUserlist"),tr("&Export to file"), hb_exportbuttons, "tofile");
	// end export buttons
	
	// buttons
	QHBox *bottom = new QHBox(center);
	QWidget *w_blank4 = new QWidget(bottom);
	bottom->setSpacing(5);
	w_blank4->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons
	
	// connect
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(pb_fetch, SIGNAL(clicked()), this, SLOT(startImportTransfer()));
	connect(pb_file, SIGNAL(clicked()), this, SLOT(fromfile()));
	connect(pb_save, SIGNAL(clicked()), this, SLOT(makeUserlist()));
	connect(pb_merge, SIGNAL(clicked()), this, SLOT(updateUserlist()));
	connect(pb_send, SIGNAL(clicked()), this, SLOT(startExportTransfer()));	
	connect(pb_tofile, SIGNAL(clicked()), this, SLOT(ExportToFile()));
	connect(pb_delete, SIGNAL(clicked()), this, SLOT(clean()));

	connect(gadu, SIGNAL(userListExported(bool)), this, SLOT(userListExported(bool)));
	connect(gadu, SIGNAL(userListCleared(bool)), this, SLOT(userListCleared(bool)));
	connect(gadu, SIGNAL(userListImported(bool, UserList&)), this, SLOT(userListImported(bool, UserList&)));
	// end connect
	
 	loadGeometry(this, "General", "ImportExportDialogGeometry", 0, 0, 560, 450);
}

UserlistImportExport::~UserlistImportExport() 
{
	kdebugf();
	saveGeometry(this, "General", "ImportExportDialogGeometry");
}

void UserlistImportExport::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void UserlistImportExport::fromfile() {
	kdebugf();
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (fname.length())
	{
		QFile file(fname);
 		if (file.open(IO_ReadOnly))
		{
			QTextStream stream(&file);
			gadu->streamToUserList(stream, importedUserlist);
			file.close();

			for (unsigned int i = 0; i < importedUserlist.count(); i++)
				new QListViewItem(lv_userlist, QString::number(importedUserlist[i].uin),
					importedUserlist[i].nickname,   importedUserlist[i].altnick,
					importedUserlist[i].first_name, importedUserlist[i].last_name,
					importedUserlist[i].mobile,     importedUserlist[i].group(),
					importedUserlist[i].email);
		}
		else
			MessageBox::wrn(tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
	}
}

void UserlistImportExport::startImportTransfer() {
	kdebugf();
	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
	{
		MessageBox::wrn(tr("Cannot import user list from server in offline mode"));
		return;
	}

	if (gadu->doImportUserList())
		pb_fetch->setEnabled(false);
}

void UserlistImportExport::makeUserlist() {
	unsigned int i;
	
	kdebugf();
	
	if (!MessageBox::ask(tr("This operation will delete your current user list. Are you sure you want this?")))
		return;

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
	l_itemscount->setText(tr("%1 entries will be exported").arg(userlist.count()));
	kdebugf2();
}

void UserlistImportExport::updateUserlist() {
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
	l_itemscount->setText(tr("%1 entries will be exported").arg(userlist.count()));
	kdebugf2();
}

void UserlistImportExport::userListImported(bool ok, UserList& userList)
{
	kdebugf();

	importedUserlist = userList;
	lv_userlist->clear();

	pb_fetch->setEnabled(true);

	if (ok)
		for (unsigned int i = 0; i < userList.count(); i++)
			new QListViewItem(lv_userlist, QString::number(userList[i].uin), userList[i].nickname, userList[i].altnick, userList[i].first_name,
				userList[i].last_name, userList[i].mobile, userList[i].group(), userList[i].email);
}

void UserlistImportExport::startExportTransfer()
{
	kdebugf();

	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
	{
		MessageBox::wrn(tr("Cannot export user list to server in offline mode"));
		return;
	}

	if (gadu->doExportUserList(userlist))
	{
		pb_send->setEnabled(false);
		pb_delete->setEnabled(false);
		pb_tofile->setEnabled(false);	
	}
}

void UserlistImportExport::ExportToFile(void)
{
	kdebugf();
	QString contacts;
	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);	

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (fname.length()) {
		contacts = gadu->userListToString(userlist);

		QFile file(fname);
		if (file.open(IO_WriteOnly)) {
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
			stream << contacts;
			file.close();
			MessageBox::msg(tr("Your userlist has been successfully exported to file"));
			}
		else
			MessageBox::wrn(tr("The application encountered an internal error\nThe export userlist to file was unsuccessful"));
		}

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
}

void UserlistImportExport::clean() {
	kdebugf();

	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL)
	{
		MessageBox::wrn(tr("Cannot clear user list on server in offline mode"));
		return;
	}

	if (gadu->doClearUserList())
	{
		pb_send->setEnabled(false);
		pb_delete->setEnabled(false);
		pb_tofile->setEnabled(false);
	}
}

void UserlistImportExport::userListExported(bool ok)
{
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully exported to server"));
	else
		MessageBox::wrn(tr("The application encountered an internal error\nThe export was unsuccessful"));

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
}

void UserlistImportExport::userListCleared(bool ok)
{
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully deleted on server"));
	else
		MessageBox::wrn(tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"));

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
}

