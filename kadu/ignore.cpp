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
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

//
#include "kadu.h"
#include "misc.h"
#include "ignore.h"
//

Ignored::Ignored(QDialog *parent, const char *name) : QDialog (parent, name) {
	resize(180,260);
	setCaption(i18n("Manage ignored users"));
	QLabel *descr = new QLabel(i18n("Ignored Uins:"),this);

	list = new QListBox(this);

	getList();

	QPushButton *b_del = new QPushButton(i18n("Delete"),this);
	QObject::connect(b_del, SIGNAL(clicked()), this, SLOT(remove()));

	QPushButton *b_add = new QPushButton(i18n("Add"),this);
	QObject::connect(b_add, SIGNAL(clicked()), this, SLOT(add()));

	e_uin = new QLineEdit(this);
	QToolTip::add(e_uin, "Type here the UIN of the person you want to ignore");
	QLabel *l_uin = new QLabel(i18n("Uin"),this);

	QGridLayout *grid = new QGridLayout(this, 4,2,6,6);
	grid->addWidget(descr,0,0);
	grid->addMultiCellWidget(list,1,1,0,1);
	grid->addWidget(l_uin,2,0, Qt::AlignRight);
	grid->addWidget(e_uin,2,1);
	grid->addWidget(b_del,3,0);
	grid->addWidget(b_add,3,1);
}

bool isIgnored(uin_t uin) {
	return ignored.contains(uin);
}

void Ignored::add() {
	addIgnored(atoi(e_uin->text().latin1()));
	e_uin->clear();
	writeIgnored(NULL);
	getList();
}

void Ignored::getList() {
	int i, j, k;
	QString buf;
	list->clear();
	for (i = 0; i < ignored.count(); i++) {
		j = 0;
		while (j < userlist.count() && ignored[i] != userlist[j].uin)
			j++;
		if (j < userlist.count())
			buf = QString("%1 (%2)").arg(QString::number(userlist[j].uin)).arg(userlist[j].altnick);
		else
			buf = QString("%1 (?)").arg(QString::number(ignored[i]));
		list->insertItem(buf);
		}
	list->sort();
}

void Ignored::remove() {
	if (list->currentText().latin1() == NULL)
		return;

	delIgnored(list->currentText().toUInt());
	getList();
	writeIgnored(NULL);
}

void addIgnored(uin_t uin) {
	ignored.append(uin);
}

void delIgnored(uin_t uin) {
	ignored.remove(uin);
}

int writeIgnored(QString filename)
{
	QString tmp;

	if (!(tmp = ggPath("")))
		return -1;
	mkdir(tmp.local8Bit(), 0700);

	if (!filename.length()) {
		filename = ggPath("ignore");
		}

	QFile file(filename);
	if (!file.open(IO_WriteOnly))
		return -2;

//	fchmod(fileno(f), 0600);

	for (int i = 0; i < ignored.count(); i++)
		if (ignored[i])
			tmp = QString("%1\n").arg(QString::number(ignored[i]));

	file.writeBlock(tmp, tmp.length());
	file.close();

	return 0;
}

