/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <sys/stat.h>

#include "debug.h"
#include "ignore.h"
#include "userlist.h"
//

QValueList<UinsList> ignored;

Ignored::Ignored(QDialog * /*parent*/, const char * /*name*/)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose);
	setCaption(tr("Manage ignored users"));

	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	QWidget *blank=new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center=new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(icons_manager.loadIcon("ManageIgnoredWindowIcon"));
	l_info->setText(tr("This dialog box allows you to manage your ignored contacts."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)

	// our QListBox
	lb_list = new QListBox(center);
	getList();
	// end our QListBox

	//our QVGroupBox
	QVGroupBox *vgb_uin = new QVGroupBox(center);
	vgb_uin->setTitle(tr("Uin"));
	//end our QGroupBox

	QLabel *l_uin = new QLabel(tr("Type here the UIN of the person you want to ignore."), vgb_uin);
	l_uin->setAlignment(Qt::WordBreak);

	QHBox *hb_uin = new QHBox(vgb_uin);
	hb_uin->setMargin(5);
	hb_uin->setSpacing(3);

	QLabel *l_uinlabel = new QLabel(tr("Uin:"), hb_uin);
	// chyba tylko po to zeby nie pokazywal sie warning kompilatora ;)
	l_uinlabel->setAlignment(Qt::WordBreak);
	e_uin = new QLineEdit(hb_uin);
	hb_uin->setStretchFactor(e_uin, 1);

	// buttons
	QHBox *bottom=new QHBox(center);
	QWidget *blank2=new QWidget(bottom);
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_del = new QPushButton(icons_manager.loadIcon("DeleteIgnoredButton"), tr("Delete"), bottom);
	QPushButton *pb_add = new QPushButton(icons_manager.loadIcon("AddIgnoredButton"), tr("Add"), bottom);
	QPushButton *pb_close = new QPushButton(icons_manager.loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	// end buttons

	connect(pb_add, SIGNAL(clicked()), this, SLOT(add()));
	connect(pb_del, SIGNAL(clicked()), this, SLOT(remove()));
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

 	loadGeometry(this, "General", "IgnoredDialogGeometry", 0, 0, 330, 350);
	kdebugf2();
}

Ignored::~Ignored()
{
	kdebugf();
	saveGeometry(this, "General", "IgnoredDialogGeometry");
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
	UinsList uins;
	QStringList strlist = QStringList::split(";", e_uin->text());
	CONST_FOREACH(strUin, strlist)
	{
		UinType uin = (*strUin).toUInt(&ok);
		if (ok)
			uins.append(uin);
	}
	if (!uins.empty())
	{
		addIgnored(uins);
		e_uin->clear();
		writeIgnored();
		getList();
	}
	kdebugf2();
}

void Ignored::getList()
{
	kdebugf();
	lb_list->clear();
	CONST_FOREACH(ignoredList, ignored)
	{
		QStringList strlist;
		CONST_FOREACH(uin, *ignoredList)
		{
			if (userlist.containsUin(*uin))
				strlist.append(QString("%1 (%2)").arg(QString::number(*uin)).arg(userlist.byUin(*uin).altNick()));
			else
				strlist.append(QString("%1").arg(QString::number(*uin)));
		}
		lb_list->insertItem(icons_manager.loadIcon("Blocking"), strlist.join(";"));
	}
	kdebugf2();
}

void Ignored::remove()
{
	kdebugf();
	if (lb_list->currentItem() == -1)
		return;
	QStringList strlist = QStringList::split(";", lb_list->currentText());
	UinsList uins;
	CONST_FOREACH(str, strlist)
		uins.append((*str).section(' ', 0, 0).toUInt());
	delIgnored(uins);
	getList();
	writeIgnored();
	kdebugf2();
}

bool isIgnored(UinsList uins)
{
	uins.sort();
	return ignored.contains(uins);
}

void addIgnored(UinsList uins)
{
	uins.sort();
	ignored.append(uins);
}

void delIgnored(UinsList uins)
{
	uins.sort();
	ignored.remove(uins);
}

int writeIgnored(QString filename)
{
	kdebugf();
	QString tmp;

	if (!(tmp = ggPath("")))
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "failed\n");
		return 1;
	}
	mkdir(tmp.local8Bit(), 0700);

	if (filename == QString::null)
		filename = ggPath("ignore");

	QFile file(filename);
	if (!file.open(IO_WriteOnly | IO_Truncate))
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't open ignore file!\n");
		return 2;
	}

//	fchmod(fileno(f), 0600);

	QString buf;
	CONST_FOREACH(ignoreList, ignored)
	{
		QStringList list;
		CONST_FOREACH(uin, *ignoreList)
			list.append(QString::number(*uin));
		buf.append(list.join(";"));
		buf.append('\n');
	}

	if (!buf.isEmpty())
		file.writeBlock(buf, buf.length());
	file.close();

	kdebugf2();
	return 0;
}

int readIgnored()
{
	kdebugf();
	QString line, fname;
	QStringList list;

	fname = ggPath("ignore");

	QFile f(fname);
	if (!f.open(IO_ReadOnly))
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't open ignore file!\n");
		return 1;
	}

	QTextStream stream(&f);
	while ((line = stream.readLine()) != QString::null)
	{
		UinsList uins;
		list = QStringList::split(";", line);
		CONST_FOREACH(strUin, list)
			uins.append((*strUin).toUInt());
		ignored.append(uins);
	}

	f.close();

	kdebugf2();
	return 0;
}

void clearIgnored()
{
	ignored.clear();
}
