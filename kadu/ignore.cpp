/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qtooltip.h>

#include <sys/stat.h>
#include "ignore.h"
//

QValueList<UinsList> ignored;

Ignored::Ignored(QDialog *parent, const char *name) : QDialog (parent, name) {
	resize(180,260);
	setCaption(tr("Manage ignored users"));
	QLabel *descr = new QLabel(tr("Ignored Uins:"),this);

	list = new QListBox(this);

	getList();

	QPushButton *b_del = new QPushButton(tr("Delete"),this);
	QObject::connect(b_del, SIGNAL(clicked()), this, SLOT(remove()));

	QPushButton *b_add = new QPushButton(tr("Add"),this);
	QObject::connect(b_add, SIGNAL(clicked()), this, SLOT(add()));

	e_uin = new QLineEdit(this);
	QToolTip::add(e_uin, "Type here the UIN of the person you want to ignore");
	QLabel *l_uin = new QLabel(tr("Uin"),this);

	QGridLayout *grid = new QGridLayout(this, 4,2,6,6);
	grid->addWidget(descr,0,0);
	grid->addMultiCellWidget(list,1,1,0,1);
	grid->addWidget(l_uin,2,0, Qt::AlignRight);
	grid->addWidget(e_uin,2,1);
	grid->addWidget(b_del,3,0);
	grid->addWidget(b_add,3,1);
}

void Ignored::add() {
	QStringList strlist;
	strlist = QStringList::split(";", e_uin->text());
	bool ok;
	UinsList uins;
	for (int i = 0; i < strlist.count(); i++) {
		uin_t uin = strlist[i].toUInt(&ok);
		if (ok)
			uins.append(uin);
		}
	if (uins.count()) {
		addIgnored(uins);
		e_uin->clear();
		writeIgnored();
		getList();
		}
}

void Ignored::getList() {
	int i, j, k;
	list->clear();
	for (i = 0; i < ignored.count(); i++) {
		QStringList strlist;
		for (j = 0; j < ignored[i].count(); j++) {
			k = 0;
			while (k < userlist.count() && ignored[i][j] != userlist[k].uin)
				k++;
			if (k < userlist.count())
				strlist.append(QString("%1 (%2)").arg(QString::number(userlist[k].uin)).arg(userlist[k].altnick));
			else
				strlist.append(QString("%1").arg(QString::number(ignored[i][j])));
			}
		list->insertItem(strlist.join(";"));
		}
}

void Ignored::remove() {
	if (list->currentItem() == -1)
		return;
	QStringList strlist;
	strlist = QStringList::split(";", list->currentText());
	UinsList uins;
	for (int i = 0; i < strlist.count(); i++) {
		QString str;
		str = strlist[i].section(' ', 0, 0);
		uins.append(str.toUInt());
		}
	delIgnored(uins);
	getList();
	writeIgnored();
}

bool isIgnored(UinsList uins) {
	uins.sort();
	return ignored.contains(uins);
}


void addIgnored(UinsList uins) {
	uins.sort();
	ignored.append(uins);
}

void delIgnored(UinsList uins) {
	uins.sort();
	ignored.remove(uins);
}

int writeIgnored(QString filename)
{
	QString tmp;

	if (!(tmp = ggPath("")))
		return 1;
	mkdir(tmp.local8Bit(), 0700);

	if (filename == QString::null)
		filename = ggPath("ignore");

	QFile file(filename);
	if (!file.open(IO_WriteOnly | IO_Truncate))
		return 2;

//	fchmod(fileno(f), 0600);

	QString buf;
	for (int i = 0; i < ignored.count(); i++) {
		QStringList list;
		for (int j = 0; j < ignored[i].count(); j++)
			list.append(QString::number(ignored[i][j]));
		buf.append(list.join(";"));
		}

	if (buf.length())
		file.writeBlock(buf, buf.length());
	file.close();

	return 0;
}

int readIgnored() {
	QString line, fname;
	QStringList list;

	fname = ggPath("ignore");

	QFile f(fname);
	if (!f.open(IO_ReadOnly))
		return 1;

	QTextStream stream(&f);
	while ((line = stream.readLine()) != QString::null) {
		list = QStringList::split(";", line);
		UinsList uins;
		for (int i = 0; i < list.count(); i++)
			uins.append(list[i].toUInt());
		ignored.append(uins);
		}

	f.close();

	return 0;
}

void clearIgnored() {
	ignored.clear();
}
