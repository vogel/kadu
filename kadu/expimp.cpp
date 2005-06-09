/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfiledialog.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <stdlib.h>

#include "debug.h"
#include "expimp.h"
#include "gadu.h"
#include "ignore.h"
#include "kadu.h"
#include "message_box.h"
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

 	loadGeometry(this, "General", "ImportExportDialogGeometry", 0, 0, 640, 450);
	kdebugf2();
}

UserlistImportExport::~UserlistImportExport()
{
	kdebugf();
	saveGeometry(this, "General", "ImportExportDialogGeometry");
	kdebugf2();
}

void UserlistImportExport::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void UserlistImportExport::fromfile()
{
	kdebugf();
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (!fname.isEmpty())
	{
		QFile file(fname);
 		if (file.open(IO_ReadOnly))
		{
			QTextStream stream(&file);
			gadu->streamToUserList(stream, importedUserlist);
			file.close();

			CONST_FOREACH(i, importedUserlist)
				new QListViewItem(lv_userlist, QString::number((*i).uin()),
					(*i).nickName(),  (*i).altNick(),
					(*i).firstName(), (*i).lastName(),
					(*i).mobile(),    (*i).group(),
					(*i).email());
		}
		else
			MessageBox::wrn(tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"));
	}
	kdebugf2();
}

void UserlistImportExport::startImportTransfer()
{
	kdebugf();
	if (gadu->status().isOffline())
	{
		MessageBox::wrn(tr("Cannot import user list from server in offline mode"));
		return;
	}

	if (gadu->doImportUserList())
		pb_fetch->setEnabled(false);
	kdebugf2();
}

void UserlistImportExport::makeUserlist()
{
	kdebugf();

	if (!MessageBox::ask(tr("This operation will delete your current user list. Are you sure you want this?")))
		return;

	userlist = importedUserlist;

	clearIgnored();
	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();
	CONST_FOREACH(user, userlist)
		kadu->userbox()->addUser((*user).altNick());

	UserBox::all_refresh();

	userlist.writeToConfig();
	l_itemscount->setText(tr("%1 entries will be exported").arg(userlist.count()));
	kdebugf2();
}

void UserlistImportExport::updateUserlist()
{
	kdebugf();

	userlist.merge(importedUserlist);

	kadu->userbox()->clear();
	kadu->userbox()->clearUsers();

	CONST_FOREACH(user, userlist)
		kadu->userbox()->addUser((*user).altNick());

	UserBox::all_refresh();

	userlist.writeToConfig();
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
		CONST_FOREACH(user, userList)
			new QListViewItem(lv_userlist, (*user).uin() ? QString::number((*user).uin()) : QString(), (*user).nickName(),
				(*user).altNick(), (*user).firstName(), (*user).lastName(), (*user).mobile(), (*user).group(), (*user).email());
	kdebugf2();
}

void UserlistImportExport::startExportTransfer()
{
	kdebugf();

	if (gadu->status().isOffline())
	{
		MessageBox::wrn(tr("Cannot export user list to server in offline mode"));
		kdebugf2();
		return;
	}

	if (gadu->doExportUserList(userlist))
	{
		pb_send->setEnabled(false);
		pb_delete->setEnabled(false);
		pb_tofile->setEnabled(false);
	}
	kdebugf2();
}

void UserlistImportExport::ExportToFile(void)
{
	kdebugf();
	QString contacts;
	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);

	QString fname = QFileDialog::getSaveFileName("/", QString::null,this);
	if (!fname.isEmpty())
	{
		contacts = gadu->userListToString(userlist);

		QFile file(fname);
		if (file.open(IO_WriteOnly))
		{
			QTextStream stream(&file);
			stream.setCodec(codec_latin2);
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
	kdebugf2();
}

void UserlistImportExport::clean()
{
	kdebugf();

	if (gadu->status().isOffline())
	{
		MessageBox::wrn(tr("Cannot clear user list on server in offline mode"));
		kdebugf2();
		return;
	}

	if (gadu->doClearUserList())
	{
		pb_send->setEnabled(false);
		pb_delete->setEnabled(false);
		pb_tofile->setEnabled(false);
	}
	kdebugf2();
}

void UserlistImportExport::userListExported(bool ok)
{
	kdebugf();
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully exported to server"));
	else
		MessageBox::wrn(tr("The application encountered an internal error\nThe export was unsuccessful"));

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
	kdebugf2();
}

void UserlistImportExport::userListCleared(bool ok)
{
	kdebugf();
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully deleted on server"));
	else
		MessageBox::wrn(tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"));

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
	kdebugf2();
}

