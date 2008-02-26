/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <q3simplerichtext.h>
#include <q3vbox.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3ValueList>
#include <QKeyEvent>

#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "ignore.h"
#include "misc.h"
#include "userlist.h"

Ignored::Ignored(QWidget *parent, const char *name) : Q3HBox(parent, name, Qt::WType_TopLevel | Qt::WDestructiveClose),
	lb_list(0), e_uin(0), layoutHelper(new LayoutHelper())
{
	kdebugf();
	setCaption(tr("Manage ignored users"));
	layout()->setResizeMode(QLayout::Minimum);

	// create main QLabel widgets (icon and app info)
	Q3VBox *left = new Q3VBox(this);
	//left->layout()->setResizeMode(QLayout::Minimum);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	Q3VBox *center = new Q3VBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager->loadIcon("ManageIgnoredWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage your ignored contacts."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QListBox
	lb_list = new Q3ListBox(center);
	getList();
	// end our QListBox

	//our QVGroupBox
	Q3VGroupBox *vgb_uin = new Q3VGroupBox(center);
	vgb_uin->setTitle(tr("Uin"));
	//end our QGroupBox

	QLabel *l_uin = new QLabel(tr("Type here the UIN of the person you want to ignore."), vgb_uin);
	l_uin->setAlignment(Qt::WordBreak);

	Q3HBox *hb_uin = new Q3HBox(vgb_uin);
	hb_uin->setMargin(5);
	hb_uin->setSpacing(3);

	QLabel *l_uinlabel = new QLabel(tr("Uin:"), hb_uin);
	// that's only for disabling compiler warning? ;)
	l_uinlabel->setAlignment(Qt::WordBreak);
	e_uin = new QLineEdit(hb_uin);
	hb_uin->setStretchFactor(e_uin, 1);

	// buttons
	Q3HBox *bottom = new Q3HBox(center);
	QWidget *blank2 = new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_del = new QPushButton(icons_manager->loadIcon("DeleteIgnoredButton"), tr("Delete"), bottom);
	QPushButton *pb_add = new QPushButton(icons_manager->loadIcon("AddIgnoredButton"), tr("Add"), bottom);
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_add, SIGNAL(clicked()), this, SLOT(add()));
	connect(pb_del, SIGNAL(clicked()), this, SLOT(remove()));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

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
		lb_list->insertItem(icons_manager->loadIcon("Blocking"), strlist.join(";"));
	}
	kdebugf2();
}

void Ignored::remove()
{
	kdebugf();
	if (lb_list->currentItem() == -1)
		return;
	QStringList strlist = QStringList::split(";", lb_list->currentText());
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



Q3ValueList<QPair<UserListElements, bool> > IgnoredManager::Ignored;

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

const Q3ValueList<QPair<UserListElements, bool> > & IgnoredManager::getList()
{
	return Ignored;
}
