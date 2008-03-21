/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "ignore.h"
#include "misc.h"
#include "userlist.h"

Ignored::Ignored(QWidget *parent, const char *name) : QWidget(parent, name, Qt::Window),
	lb_list(0), e_uin(0), layoutHelper(new LayoutHelper())
{
	kdebugf();
	setWindowTitle(tr("Manage ignored users"));
	setAttribute(Qt::WA_DeleteOnClose);

	//layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(icons_manager->loadPixmap("ManageIgnoredWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout;
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);
	left->setLayout(left_layout);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();
	l_info->setText(tr("This dialog box allows you to manage your ignored contacts."));
	l_info->setAlignment(Qt::TextWordWrap);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)

	// our QListBox
	lb_list = new QListWidget();
	getList();
	// end our QListBox

	//our QVGroupBox
	QGroupBox *vgb_uin = new QGroupBox(tr("Uin"));
	QVBoxLayout *uin_layout = new QVBoxLayout;
	vgb_uin->setLayout(uin_layout);

	//end our QGroupBox

	QLabel *l_uin = new QLabel(tr("Type here the UIN of the person you want to ignore."));
	l_uin->setAlignment(Qt::TextWordWrap);

	QWidget *hb_uin = new QWidget;

	QLabel *l_uinlabel = new QLabel(tr("Uin:"));
	e_uin = new QLineEdit();

	QHBoxLayout* hb_uin_layout = new QHBoxLayout;
	hb_uin_layout->setSpacing(3);
	hb_uin_layout->setMargin(5);
	hb_uin_layout->addWidget(l_uinlabel);
	hb_uin_layout->addWidget(e_uin);
	hb_uin_layout->setStretchFactor(e_uin, 1);

	hb_uin->setLayout(hb_uin_layout);

	uin_layout->addWidget(l_uin);
	uin_layout->addWidget(hb_uin);

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_del = new QPushButton(icons_manager->loadIcon("DeleteIgnoredButton"), tr("Delete"), bottom);
	connect(pb_del, SIGNAL(clicked()), this, SLOT(remove()));

	QPushButton *pb_add = new QPushButton(icons_manager->loadIcon("AddIgnoredButton"), tr("Add"), bottom);
	connect(pb_add, SIGNAL(clicked()), this, SLOT(add()));

	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout;
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_del);
	bottom_layout->addWidget(pb_add);
	bottom_layout->addWidget(pb_close);

	bottom->setLayout(bottom_layout);

	// end buttons
	QVBoxLayout *center_layout = new QVBoxLayout;
	center_layout->addWidget(l_info);
	center_layout->addWidget(lb_list);
	center_layout->addWidget(vgb_uin);
	center_layout->addWidget(bottom);

	center->setLayout(center_layout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(left);
	layout->addWidget(center);

	setLayout(layout);

	layoutHelper->addLabel(l_info);
	layoutHelper->addLabel(l_uin);

 	loadGeometry(this, "General", "IgnoredDialogGeometry", 0, 30, 330, 350);
	kdebugf2();
}

Ignored::~Ignored()
{
	kdebugf();
// 	saveGeometry(this, "General", "IgnoredDialogGeometry");
	delete layoutHelper;
	kdebugf2();
}

void Ignored::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
	if ((ke_event->key() == Qt::Key_Return || ke_event->key() == Qt::Key_Enter) && e_uin->hasFocus())
		add();
}

void Ignored::add()
{
	kdebugf();
	bool ok;
	UserListElements users;
	QStringList strlist = QStringList::split(";", e_uin->text());
	CONST_FOREACH(strUin, strlist)
	{
		UinType uin = (*strUin).toUInt(&ok);
		if (ok)
			users.append(userlist->byID("Gadu", QString::number(uin)));
	}
	if (!users.empty())
	{
		IgnoredManager::insert(users);
		e_uin->clear();
		IgnoredManager::writeToConfiguration();
		getList();
	}
	kdebugf2();
}

void Ignored::getList()
{
	kdebugf();
	lb_list->clear();
	CONST_FOREACH(ignoredList, IgnoredManager::getList())
	{
		QStringList strlist;
		CONST_FOREACH(user, (*ignoredList).first)
		{
			if (userlist->contains(*user))
				strlist.append(QString("%1 (%2)").arg((*user).ID("Gadu")).arg((*user).altNick()));
			else
				strlist.append(QString("%1").arg((*user).ID("Gadu")));
		}
		lb_list->addItem(new QListWidgetItem(icons_manager->loadPixmap("Blocking"), strlist.join(";")));
	}
	kdebugf2();
}

void Ignored::remove()
{
	kdebugf();
	if (!lb_list->currentItem())
		return;
	QStringList strlist = QStringList::split(";", lb_list->currentItem()->text());
	UserListElements users;
	CONST_FOREACH(str, strlist)
		users.append(userlist->byID("Gadu", (*str).section(' ', 0, 0)));
	IgnoredManager::remove(users);
	getList();
	IgnoredManager::writeToConfiguration();
	kdebugf2();
}

void Ignored::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}



QList<QPair<UserListElements, bool> > IgnoredManager::Ignored;

void IgnoredManager::loadFromConfiguration()
{
	kdebugf();

	QDomElement ignored_elem = xml_config_file->findElement(xml_config_file->rootElement(), "Ignored");

	if (ignored_elem.isNull())
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't find Ignored element!\n");
		return;
	}

	QDomNodeList ignored_groups = ignored_elem.elementsByTagName("IgnoredGroup");
	for (int i = 0; i < ignored_groups.count(); i++)
	{
		QDomElement ignored_group = ignored_groups.item(i).toElement();
		UserListElements users;
		QDomNodeList ignored_contacts = ignored_group.elementsByTagName("IgnoredContact");
		for (int j = 0; j < ignored_contacts.count(); j++)
		{
			QDomElement ignored_contact = ignored_contacts.item(j).toElement();
			users.append(userlist->byID("Gadu", ignored_contact.attribute("uin")));
		}
		users.sort();
		Ignored.append(qMakePair(users, false));
	}

	kdebugf2();
}

void IgnoredManager::writeToConfiguration()
{
	kdebugf();
	QDomElement ignored_elem = xml_config_file->accessElement(xml_config_file->rootElement(), "Ignored");
	xml_config_file->removeChildren(ignored_elem);

	CONST_FOREACH(ignoreList, Ignored)
	{
		if ((*ignoreList).second)
			continue;

		QDomElement ignored_group_elem = xml_config_file->createElement(ignored_elem, "IgnoredGroup");
		CONST_FOREACH(user, (*ignoreList).first)
		{
			QDomElement ignored_contact_elem = xml_config_file->createElement(ignored_group_elem, "IgnoredContact");
			ignored_contact_elem.setAttribute("uin", (*user).ID("Gadu"));
		}
	}

	kdebugf2();
}

void IgnoredManager::insert(UserListElements users, bool temporary)
{
	users.sort();
	Ignored.append(qMakePair(users, temporary));
}

void IgnoredManager::remove(UserListElements users)
{
	users.sort();
	Ignored.remove(qMakePair(users, false));
	Ignored.remove(qMakePair(users, true));
}

bool IgnoredManager::isIgnored(UserListElements users)
{
	users.sort();
	return (Ignored.contains(qMakePair(users, false)) || Ignored.contains(qMakePair(users, true)));
}

void IgnoredManager::clear()
{
	Ignored.clear();
}

const QList<QPair<UserListElements, bool> > & IgnoredManager::getList()
{
	return Ignored;
}
