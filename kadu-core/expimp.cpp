/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>

#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>

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
	QWidget(parent, name, Qt::Window),
	pb_fetch(0), importedUserlist(), pb_send(0), pb_delete(0), pb_tofile(0),
	l_itemscount(0), layoutHelper(new LayoutHelper()), lv_userlist(0)
{
	kdebugf();
	setWindowTitle(tr("Import / export userlist"));
	setAttribute(Qt::WA_DeleteOnClose);
// 	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget;

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("ImportExportWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);

	left->setLayout(left_layout);

	QWidget *center = new QWidget;
	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->setSpacing(5);
	QLabel *l_info = new QLabel;
	l_info->setText(tr("This dialog box allows you to import and export your buddy list to a server or a file."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QListView
	// our QGroupBox
	QGroupBox *gb_import = new QGroupBox;
	QVBoxLayout *import_layout = new QVBoxLayout;
	import_layout->setSpacing(5);
	gb_import->setTitle(tr("Import userlist"));
	// end our QGroupBox
	lv_userlist = new Q3ListView;
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
	QWidget *importbuttons = new QWidget;
	QHBoxLayout *importbuttons_layout = new QHBoxLayout;
	importbuttons_layout->setSpacing(5);

	QWidget *w_blank2 = new QWidget;
	w_blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	pb_fetch = new QPushButton(icons_manager->loadIcon("FetchUserList"), tr("&Fetch userlist"), this, "fetch");
	QPushButton *pb_file = new QPushButton(icons_manager->loadIcon("ImportFromFile"), tr("&Import from file"), this, "file");
	QPushButton *pb_save = new QPushButton(icons_manager->loadIcon("SaveUserlist"), tr("&Save results"), this, "save");
	QPushButton *pb_merge = new QPushButton(icons_manager->loadIcon("MergeUserlist"), tr("&Merge results"), this, "merge");
	// end buttons
	
	importbuttons_layout->addWidget(w_blank2);
	importbuttons_layout->addWidget(pb_fetch);
	importbuttons_layout->addWidget(pb_file);
	importbuttons_layout->addWidget(pb_save);
	importbuttons_layout->addWidget(pb_merge);
	importbuttons->setLayout(importbuttons_layout);
	
	import_layout->addWidget(lv_userlist);
	import_layout->addWidget(importbuttons);
	gb_import->setLayout(import_layout);

	// our QGroupBox
	QGroupBox *gb_export = new QGroupBox;
	QVBoxLayout *export_layout = new QVBoxLayout;
	export_layout->setSpacing(5);
	gb_export->setTitle(tr("Export userlist"));
	// end our QGroupBox

	l_itemscount = new QLabel;
	updateUserListCount();

	// export buttons
	QWidget *exportbuttons = new QWidget;
	QHBoxLayout *exportbuttons_layout = new QHBoxLayout;
	exportbuttons_layout->setSpacing(5);
	QWidget *w_blank3 = new QWidget;
	w_blank3->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	pb_send = new QPushButton(icons_manager->loadIcon("SendUserlist"), tr("Se&nd userlist"), this, "send");
	pb_delete = new QPushButton(icons_manager->loadIcon("DeleteUserlist"), tr("&Delete userlist"), this, "delete");
	pb_tofile = new QPushButton(icons_manager->loadIcon("ExportUserlist"), tr("&Export to file"), this, "tofile");
	// end export buttons

	exportbuttons_layout->addWidget(w_blank3);
	exportbuttons_layout->addWidget(pb_send);
	exportbuttons_layout->addWidget(pb_delete);
	exportbuttons_layout->addWidget(pb_tofile);
	exportbuttons->setLayout(exportbuttons_layout);
	
	export_layout->addWidget(l_itemscount);
	export_layout->addWidget(exportbuttons);
	gb_export->setLayout(export_layout);

	// buttons
	QWidget *bottom = new QWidget;
	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->setSpacing(5);
	QWidget *w_blank4 = new QWidget;
	w_blank4->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), this, "close");
	// end buttons
	bottom_layout->addWidget(w_blank4);
	bottom_layout->addWidget(pb_close);
	bottom->setLayout(bottom_layout);

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
	connect(gadu, SIGNAL(userListImported(bool, QList<UserListElement>)), this, SLOT(userListImported(bool, QList<UserListElement>)));
	// end connect

	center_layout->addWidget(l_info);
	center_layout->addWidget(gb_import);
	center_layout->addWidget(gb_export);
	center_layout->addWidget(bottom);
	center->setLayout(center_layout);

	QWidget *sub = new QWidget;
	QHBoxLayout *sub_layout = new QHBoxLayout;
	sub_layout->addWidget(left);
	sub_layout->addWidget(center);
	sub->setLayout(sub_layout);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(sub);
	layout->addWidget(bottom);

	setLayout(layout);

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
	QString fname = QFileDialog::getOpenFileName("/", QString::null, this);
	if (!fname.isEmpty())
	{
		QFile file(fname);
 		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&file);
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

void UserlistImportExport::userListImported(bool ok, QList<UserListElement> userList)
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

	QString fname = QFileDialog::getSaveFileName(QString(getenv("HOME")), QString::null, this);
	if (!fname.isEmpty())
	{
		QFile file(fname);
		if ((!file.exists()) || (MessageBox::ask(tr("File exists. Are you sure you want to overwrite it?"), QString::null, this)))
		{
			if (file.open(QIODevice::WriteOnly))
			{
				QTextStream stream(&file);
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

