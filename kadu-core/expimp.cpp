/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtGui/QFileDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

#include "../modules/gadu_protocol/gadu-protocol.h"
#include "../modules/gadu_protocol/helpers/gadu-list-helper.h"

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/group.h"
#include "contacts/account-data/contact-account-data.h"
#include "gui/windows/message-box.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-list-service.h"

#include "debug.h"
#include "icons-manager.h"
#include "ignore.h"
#include "misc/misc.h"

#include "expimp.h"

UserlistImportExport::UserlistImportExport(QWidget *parent)
	: QWidget(parent, Qt::Window), pb_fetch(0),
		pb_send(0), pb_delete(0), pb_tofile(0), l_itemscount(0), lv_userlist(0)
{
	kdebugf();
	setWindowTitle(tr("Import / export userlist"));
	setAttribute(Qt::WA_DeleteOnClose);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget;

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(IconsManager::instance()->loadPixmap("ImportExportWindowIcon"));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addStretch();

	left->setLayout(left_layout);

	QWidget *center = new QWidget;
	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->setSpacing(5);
	QLabel *l_info = new QLabel;
	l_info->setText(tr("This dialog box allows you to import and export your buddy list to a server or a file."));
	l_info->setWordWrap(true);
#ifndef Q_OS_MAC
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	// end create main QLabel widgets (icon and app info)

	// our QListView
	// our QGroupBox
	QGroupBox *gb_import = new QGroupBox;
	QVBoxLayout *import_layout = new QVBoxLayout;
	import_layout->setSpacing(5);
	gb_import->setTitle(tr("Import userlist"));
	// end our QGroupBox
	lv_userlist = new QTreeWidget(this);

	QStringList headers;
	headers << tr("UIN") << tr("Nickname") << tr("Disp. nick") << tr("Name") << tr("Surname") << tr("Mobile no.") << tr("Group") << tr("Email");
	lv_userlist->setHeaderLabels(headers);
	lv_userlist->setAllColumnsShowFocus(true);
	lv_userlist->setIndentation(false);
	// end our QListView

	// buttons
	QWidget *importbuttons = new QWidget(this);
	QHBoxLayout *importbuttons_layout = new QHBoxLayout(importbuttons);
	importbuttons_layout->setSpacing(5);

	pb_fetch = new QPushButton(IconsManager::instance()->loadIcon("FetchUserList"), tr("&Fetch userlist"), this);
	QPushButton *pb_file = new QPushButton(IconsManager::instance()->loadIcon("ImportFromFile"), tr("&Import from file"), this);
	QPushButton *pb_save = new QPushButton(IconsManager::instance()->loadIcon("SaveUserlist"), tr("&Save results"), this);
	QPushButton *pb_merge = new QPushButton(IconsManager::instance()->loadIcon("MergeUserlist"), tr("&Merge results"), this);
	// end buttons
	
	importbuttons_layout->addStretch();
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

	pb_send = new QPushButton(IconsManager::instance()->loadIcon("SendUserlist"), tr("Se&nd userlist"), this);
	pb_delete = new QPushButton(IconsManager::instance()->loadIcon("DeleteUserlist"), tr("&Delete userlist"), this);
	pb_tofile = new QPushButton(IconsManager::instance()->loadIcon("ExportUserlist"), tr("&Export to file"), this);
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

	QPushButton *pb_close = new QPushButton(IconsManager::instance()->loadIcon("CloseWindow"), tr("&Close"), this);
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

	if (AccountManager::instance()->defaultAccount().isNull())
	{
		deleteLater();
		return;
	}

	Protocol *gadu = AccountManager::instance()->defaultAccount().protocolHandler();
	ContactListService *manager = gadu->contactListService();
	connect(manager, SIGNAL(contactListExported(bool)), this, SLOT(contactListExported(bool)));
	connect(manager, SIGNAL(contactListImported(bool, ContactList)),
		this, SLOT(contactListImported(bool, ContactList)));
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

 	loadWindowGeometry(this, "General", "ImportExportDialogGeometry", 0, 50, 640, 450);
	kdebugf2();
}

void UserlistImportExport::updateUserListCount()
{
// TODO: 0.6.6
// 	int realUserCount = 0;
// 	foreach(const UserListElement &user, *userlist)
// 		if (!user.isAnonymous())
// 			++realUserCount;
// 	l_itemscount->setText(tr("%1 entries will be exported").arg(realUserCount));
}

UserlistImportExport::~UserlistImportExport()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ImportExportDialogGeometry");
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

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount().protocolHandler());

	QString fname = QFileDialog::getOpenFileName(this, QString::null, "/");
	if (!fname.isEmpty())
	{
		QFile file(fname);
 		if (file.open(QIODevice::ReadOnly))
		{
// TODO: 0.6.6
// 			QTextStream stream(&file);
// 			importedUserlist = gadu->streamToUserList(stream);
// 			file.close();
// 
// 			foreach(const UserListElement &user, importedUserlist)
// 			{
// 				QString id;
// 				if (user.usesProtocol("Gadu"))
// 					id = user.ID("Gadu");
// 
// 				QStringList values;
// 				values << id << user.nickName() << user.altNick() << user.firstName() << user.lastName() << user.mobile() <<
// 					user.data("Groups").toStringList().join(",") << user.email();
// 				new QTreeWidgetItem(lv_userlist, values);
// 			}
		}
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe import userlist from file was unsuccessful"), false, "Critical", this);
	}
	kdebugf2();
}

void UserlistImportExport::startImportTransfer()
{
	kdebugf();

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount().protocolHandler());
	if (!gadu->isConnected())
	{
		MessageBox::msg(tr("Cannot import user list from server in offline mode"), false, "Critical", this);
		return;
	}

	pb_fetch->setEnabled(false);

	ContactListService *manager = gadu->contactListService();
	manager->importContactList();

	kdebugf2();
}

void UserlistImportExport::makeUserlist()
{
	kdebugf();

	if (!MessageBox::ask(tr("This operation will delete your current user list. Are you sure you want this?")))
		return;

	// userlist->clear();
	// userlist->merge(importedUserlist);

// TODO: 0.6.6
//	IgnoredManager::clear();
// TODO: 0.6.6
// 	userlist->writeToConfig();
	updateUserListCount();

	kdebugf2();
}

void UserlistImportExport::updateUserlist()
{
	kdebugf();
// TODO: 0.6.6
// 	userlist->merge(importedUserlist);
// 	userlist->writeToConfig();
	updateUserListCount();
	kdebugf2();
}

void UserlistImportExport::contactListImported(bool ok, ContactList contacts)
{
	kdebugf();

	ImprotedContacts = contacts;
	lv_userlist->clear();

	pb_fetch->setEnabled(true);

	if (!ok)
		return;

	Account account = AccountManager::instance()->defaultAccount();
	foreach (Contact contact, contacts)
	{
		QString id;
		if (contact.hasAccountData(account))
			id = contact.accountData(account)->id();

		QStringList groups;
		foreach (Group *group, contact.groups())
			groups << group->name();

		QStringList values;
		values
			<< id
			<< contact.nickName()
			<< contact.display()
			<< contact.firstName()
			<< contact.lastName()
			<< contact.mobile()
			<< groups.join(",") 
			<< contact.email();
		new QTreeWidgetItem(lv_userlist, values);
	}

	kdebugf2();
}

void UserlistImportExport::startExportTransfer()
{
	kdebugf();

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount().protocolHandler());
	if (!gadu->isConnected())
	{
		MessageBox::msg(tr("Cannot export user list to server in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);

	ContactListService *manager = gadu->contactListService();
	Clear = false;
	manager->exportContactList();

	kdebugf2();
}

void UserlistImportExport::ExportToFile(void)
{
	kdebugf();
	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);

	Account account = AccountManager::instance()->defaultAccount();
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(account.protocolHandler());

	QString fname = QFileDialog::getSaveFileName(this, QString::null, QString(getenv("HOME")));
	if (!fname.isEmpty())
	{
		QFile file(fname);
		if ((!file.exists()) || (MessageBox::ask(tr("File exists. Are you sure you want to overwrite it?"), QString::null, this)))
		{
			if (file.open(QIODevice::WriteOnly))
			{
				QTextStream stream(&file);
				stream.setCodec(codec_latin2);
				stream << GaduListHelper::contactListToString(account, ContactManager::instance()->contacts(account));
				file.close();
				MessageBox::msg(tr("Your userlist has been successfully exported to file"), false, "Information", this);
			}
			else
				MessageBox::msg(tr("The application encountered an internal error\nThe export userlist to file was unsuccessful"), false, "Critical", this);
		}
		else
			QTimer::singleShot(0, this, SLOT(ExportToFile()));
	}

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);

	kdebugf2();
}

void UserlistImportExport::clean()
{
	kdebugf();

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount().protocolHandler());
	if (!gadu->isConnected())
	{
		MessageBox::msg(tr("Cannot clear user list on server in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	pb_send->setEnabled(false);
	pb_delete->setEnabled(false);
	pb_tofile->setEnabled(false);

	ContactListService *manager = gadu->contactListService();
	Clear = true;
	manager->exportContactList(ContactList());

	kdebugf2();
}

void UserlistImportExport::contactListExported(bool ok)
{
	kdebugf();

	if (Clear)
		if (ok)
			MessageBox::msg(tr("Your userlist has been successfully deleted on server"), false, "Infromation", this);
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"), false, "Critical", this);
	else
		if (ok)
			MessageBox::msg(tr("Your userlist has been successfully exported to server"), false, "Information", this);
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe export was unsuccessful"), false, "Critical", this);

	pb_send->setEnabled(true);
	pb_delete->setEnabled(true);
	pb_tofile->setEnabled(true);

	kdebugf2();
}
