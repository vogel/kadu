/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>

#include "../modules/gadu_protocol/gadu.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "ignore.h"
#include "misc.h"
#include "xml_config_file.h"

Ignored::Ignored(QWidget *parent, const char *name) : QWidget(parent, name, Qt::Window),
	lb_list(0), e_uin(0)
{
// 	kdebugf();
// 	setWindowTitle(tr("Manage ignored users"));
// 	setAttribute(Qt::WA_DeleteOnClose);
// 
// 	// create main QLabel widgets (icon and app info)
// 	QWidget *left = new QWidget();
// 
// 	QLabel *l_icon = new QLabel;
// 	l_icon->setPixmap(icons_manager->loadPixmap("ManageIgnoredWindowIcon"));
// 
// 	QVBoxLayout *left_layout = new QVBoxLayout;
// 	left_layout->addWidget(l_icon);
// 	left_layout->addStretch();
// 	left->setLayout(left_layout);
// 
// 	QWidget *center = new QWidget;
// 
// 	QLabel *l_info = new QLabel();
// 	l_info->setText(tr("This dialog box allows you to manage your ignored contacts."));
// 	l_info->setWordWrap(true);
// #ifndef Q_OS_MAC
// 	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
// #endif
// 	// end create main QLabel widgets (icon and app info)
// 
// 	// our QListBox
// 	lb_list = new QListWidget();
// 	getList();
// 	// end our QListBox
// 
// 	//our QVGroupBox
// 	QGroupBox *vgb_uin = new QGroupBox(tr("Uin"));
// 	QVBoxLayout *uin_layout = new QVBoxLayout;
// 	vgb_uin->setLayout(uin_layout);
// 
// 	//end our QGroupBox
// 
// 	QLabel *l_uin = new QLabel(tr("Type here the UIN of the person you want to ignore."));
// 	l_uin->setWordWrap(true);
// 	l_uin->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
// 
// 	QWidget *hb_uin = new QWidget;
// 
// 	QLabel *l_uinlabel = new QLabel(tr("Uin:"));
// 	e_uin = new QLineEdit();
// 
// 	QHBoxLayout* hb_uin_layout = new QHBoxLayout;
// 	hb_uin_layout->setSpacing(3);
// 	hb_uin_layout->setMargin(5);
// 	hb_uin_layout->addWidget(l_uinlabel);
// 	hb_uin_layout->addWidget(e_uin);
// 	hb_uin_layout->setStretchFactor(e_uin, 1);
// 
// 	hb_uin->setLayout(hb_uin_layout);
// 
// 	uin_layout->addWidget(l_uin);
// 	uin_layout->addWidget(hb_uin);
// 
// 	// buttons
// 	QWidget *bottom = new QWidget;
// 
// 	QPushButton *pb_del = new QPushButton(icons_manager->loadIcon("DeleteIgnoredButton"), tr("Delete"), bottom);
// 	connect(pb_del, SIGNAL(clicked()), this, SLOT(remove()));
// 
// 	QPushButton *pb_add = new QPushButton(icons_manager->loadIcon("AddIgnoredButton"), tr("Add"), bottom);
// 	connect(pb_add, SIGNAL(clicked()), this, SLOT(add()));
// 
// 	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
// 	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
// 
// 	QHBoxLayout *bottom_layout = new QHBoxLayout;
// 	bottom_layout->addStretch();
// 	bottom_layout->addWidget(pb_del);
// 	bottom_layout->addWidget(pb_add);
// 	bottom_layout->addWidget(pb_close);
// 
// 	bottom->setLayout(bottom_layout);
// 
// 	// end buttons
// 	QVBoxLayout *center_layout = new QVBoxLayout;
// 	center_layout->addWidget(l_info);
// 	center_layout->addWidget(lb_list);
// 	center_layout->addWidget(vgb_uin);
// 	center_layout->addWidget(bottom);
// 
// 	center->setLayout(center_layout);
// 
// 	QHBoxLayout *layout = new QHBoxLayout;
// 	layout->addWidget(left);
// 	layout->addWidget(center);
// 
// 	setLayout(layout);
// 
//  	loadWindowGeometry(this, "General", "IgnoredDialogGeometry", 0, 50, 330, 350);
// 	kdebugf2();
}

Ignored::~Ignored()
{
// 	kdebugf();
//  	saveWindowGeometry(this, "General", "IgnoredDialogGeometry");
// 	kdebugf2();
}

void Ignored::keyPressEvent(QKeyEvent *ke_event)
{
// 	if (ke_event->key() == Qt::Key_Escape)
// 		close();
// 	if ((ke_event->key() == Qt::Key_Return || ke_event->key() == Qt::Key_Enter) && e_uin->hasFocus())
// 		add();
}

void Ignored::add()
{
// 	kdebugf();
// 	bool ok;
// 	UserListElements users;
// 	QStringList strlist = QStringList::split(";", e_uin->text());
// 	foreach(const QString &strUin, strlist)
// 	{
// 		UinType uin = strUin.toUInt(&ok);
// 		if (ok)
// 			users.append(userlist->byID("Gadu", QString::number(uin)));
// 	}
// 	if (!users.empty())
// 	{
// 		IgnoredManager::insert(users);
// 		e_uin->clear();
// 		IgnoredManager::writeToConfiguration();
// 		getList();
// 	}
// 	kdebugf2();
}

void Ignored::getList()
{
// 	kdebugf();
// 	lb_list->clear();
// 	foreach(const IgnoredManager::IgnoredEntry &ignoredEntry, IgnoredManager::getList())
// 	{
// 		QStringList strlist;
// 		foreach(const UserListElement &user, ignoredEntry.first)
// 		{
// 			if (userlist->contains(user))
// 				strlist.append(QString("%1 (%2)").arg(user.ID("Gadu")).arg(user.altNick()));
// 			else
// 				strlist.append(QString("%1").arg(user.ID("Gadu")));
// 		}
// 		lb_list->addItem(new QListWidgetItem(icons_manager->loadPixmap("Blocking"), strlist.join(";")));
// 	}
// 	kdebugf2();
}

void Ignored::remove()
{
// 	kdebugf();
// 	if (!lb_list->currentItem())
// 		return;
// 	QStringList strlist = QStringList::split(";", lb_list->currentItem()->text());
// 	UserListElements users;
// 	foreach(const QString &str, strlist)
// 		users.append(userlist->byID("Gadu", str.section(' ', 0, 0)));
// 	IgnoredManager::remove(users);
// 	getList();
// 	IgnoredManager::writeToConfiguration();
// 	kdebugf2();
}
