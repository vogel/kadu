/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>

#include <qlabel.h>
#include <qlayout.h>
#include <q3listview.h>
#include <qpushbutton.h>
#include <q3simplerichtext.h>
#include <q3vbox.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3ValueList>
#include <QKeyEvent>

#include "debug.h"
#include "expimp.h"
#include "gadu.h"
#include "icons_manager.h"
#include "ignore.h"
#include "message_box.h"
#include "misc.h"
#include "userlist.h"

void UserlistImportExport::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}

UserlistImportExport::UserlistImportExport(QWidget *parent, const char *name) :
	Q3HBox(parent, name, Qt::WType_TopLevel | Qt::WDestructiveClose),
	pb_fetch(0), importedUserlist(), pb_send(0), pb_delete(0), pb_tofile(0),
	l_itemscount(0), layoutHelper(new LayoutHelper()), lv_userlist(0)
{
	kdebugf();
	setCaption(tr("Import / export userlist"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	Q3VBox *left = new Q3VBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *w_blank = new QWidget(left);
	w_blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	Q3VBox *center = new Q3VBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadIcon("ImportExportWindowIcon"));
	l_info->setText(tr("This dialog box allows you to import and export your buddy list to a server or a file."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QListView
	// our QVGroupBox
	Q3VGroupBox *vgb_import = new Q3VGroupBox(center);
	vgb_import->setTitle(tr("Import userlist"));
	// end our QGroupBox
	lv_userlist = new Q3ListView(vgb_import);
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
	Q3HBox *hb_importbuttons = new Q3HBox(vgb_import);
	QWidget *w_blank2 = new QWidget(hb_importbuttons);
	hb_importbuttons->setSpacing(5);
	w_blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	pb_fetch = new QPushButton(icons_manager->loadIconSet("FetchUserList"), tr("&Fetch userlist"), hb_importbuttons, "fetch");
	QPushButton *pb_file = new QPushButton(icons_manager->loadIcon("ImportFromFile"), tr("&Import from file"), hb_importbuttons, "file");
	QPushButton *pb_save = new QPushButton(icons_manager->loadIcon("SaveUserlist"), tr("&Save results"), hb_importbuttons, "save");
	QPushButton *pb_merge = new QPushButton(icons_manager->loadIcon("MergeUserlist"), tr("&Merge results"), hb_importbuttons, "merge");
	// end buttons

	// our QVGroupBox
	Q3VGroupBox *vgb_export = new Q3VGroupBox(center);
	vgb_export->setTitle(tr("Export userlist"));
	// end our QGroupBox

	l_itemscount = new QLabel(vgb_export);
	updateUserListCount();

	// export buttons
	Q3HBox *hb_exportbuttons = new Q3HBox(vgb_export);
	QWidget *w_blank3 = new QWidget(hb_exportbuttons);
	hb_exportbuttons->setSpacing(5);
	w_blank3->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	pb_send = new QPushButton(icons_manager->loadIconSet("SendUserlist"),tr("Se&nd userlist"), hb_exportbuttons, "send");
	pb_delete = new QPushButton(icons_manager->loadIconSet("DeleteUserlist"),tr("&Delete userlist"), hb_exportbuttons, "delete");
	pb_tofile = new QPushButton(icons_manager->loadIconSet("ExportUserlist"),tr("&Export to file"), hb_exportbuttons, "tofile");
	// end export buttons

	// buttons
	Q3HBox *bottom = new Q3HBox(center);
	QWidget *w_blank4 = new QWidget(bottom);
	bottom->setSpacing(5);
	w_blank4->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
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
	connect(gadu, SIGNAL(userListImported(bool, Q3ValueList<UserListElement>)), this, SLOT(userListImported(bool, Q3ValueList<UserListElement>)));
	// end connect

	layoutHelper->addLabel(l_info);
 	loadGeometry(this, "General", "ImportExportDialogGeometry", 0, 30, 640, 450);
	kdebugf2();
}

void UserlistImportExport::updateUserListCount()
{
	int realUserCount = 0;
	CONST_FOREACH(user, *userlist)
		if (!(*user).isAnonymous())
			++realUserCount;
	l_itemscount->setText(tr("%1 entries will be exported").arg(realUserCount));
}

UserlistImportExport::~UserlistImportExport()
{
	kdebugf();
// 	saveGeometry(this, "General", "ImportExportDialogGeometry");
	delete layoutHelper;
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
	QString fname = Q3FileDialog::getOpenFileName("/", QString::null, this);
	if (!fname.isEmpty())
	{
		QFile file(fname);
 		if (file.open(QIODevice::ReadOnly))
		{
			Q3TextStream stream(&file);
			importedUserlist = gadu->streamToUserList(stream);
			file.close();

			CONST_FOREACH(i, importedUserlist)
			{
				QString id;
				if ((*i).usesProtocol("Gadu"))
					id = (*i).ID("Gadu");
				new Q3ListViewItem(lv_userlist, id,
					(*i).nickName(),  (*i).altNick(),
					(*i).firstName(), (*i).lastName(),
					(*i).mobile(),    (*i).data("Groups").toStringList().join(","),
					(*i).email());
			}
		}
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"), false, "Critical", this);
	}
	kdebugf2();
}

void UserlistImportExport::startImportTransfer()
{
	kdebugf();
	if (gadu->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot import user list from server in offline mode"), false, "Critical", this);
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

	userlist->clear();
	userlist->merge(importedUserlist);
	IgnoredManager::clear();
	userlist->writeToConfig();
	updateUserListCount();

	kdebugf2();
}

void UserlistImportExport::updateUserlist()
{
	kdebugf();
	userlist->merge(importedUserlist);
	userlist->writeToConfig();
	updateUserListCount();
	kdebugf2();
}

void UserlistImportExport::userListImported(bool ok, Q3ValueList<UserListElement> userList)
{
	kdebugf();

	importedUserlist = userList;
	lv_userlist->clear();

	pb_fetch->setEnabled(true);

	if (ok)
		CONST_FOREACH(user, userList)
		{
			QString id;
			if ((*user).usesProtocol("Gadu"))
				id = (*user).ID("Gadu");
			new Q3ListViewItem(lv_userlist, id, (*user).nickName(),
				(*user).altNick(), (*user).firstName(), (*user).lastName(), (*user).mobile(),
				(*user).data("Groups").toStringList().join(","), (*user).email());
		}
	kdebugf2();
}

void UserlistImportExport::startExportTransfer()
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot export user list to server in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	if (gadu->doExportUserList(*userlist))
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
	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);

	QString fname = Q3FileDialog::getSaveFileName(QString(getenv("HOME")), QString::null, this);
	if (!fname.isEmpty())
	{
		QFile file(fname);
		if ((!file.exists()) || (MessageBox::ask(tr("File exists. Are you sure you want to overwrite it?"), QString::null, this)))
		{
			if (file.open(QIODevice::WriteOnly))
			{
				Q3TextStream stream(&file);
				stream.setCodec(codec_latin2);
				stream << gadu->userListToString(*userlist);
				file.close();
				MessageBox::msg(tr("Your userlist has been successfully exported to file"), false, "Information", this);
			}
			else
				MessageBox::msg(tr("The application encountered an internal error\nThe export userlist to file was unsuccessful"), false, "Critical", this);
		}
	}

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
	kdebugf2();
}

void UserlistImportExport::clean()
{
	kdebugf();

	if (gadu->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot clear user list on server in offline mode"), false, "Critical", this);
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
		MessageBox::msg(tr("Your userlist has been successfully exported to server"), false, "Information", this);
	else
		MessageBox::msg(tr("The application encountered an internal error\nThe export was unsuccessful"), false, "Critical", this);

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
	kdebugf2();
}

void UserlistImportExport::userListCleared(bool ok)
{
	kdebugf();
	if (ok)
		MessageBox::msg(tr("Your userlist has been successfully deleted on server"), false, "Infromation", this);
	else
		MessageBox::msg(tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"), false, "Critical", this);

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);
	kdebugf2();
}

