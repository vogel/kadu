/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qhbox.h>
#include <qvbox.h>

//
#include "kadu.h"
//
#include "adduser.h"
#include "debug.h"

Adduser::Adduser(QDialog* parent, const char *name) {
	kdebug("Adduser::Adduser()\n");

	resize(350, 250);
	setCaption(i18n("Add user"));

	QGridLayout *grid = new QGridLayout(this, 9, 2, 10, 10);

	QVBox *vbox11 = new QVBox(this);
	QLabel *l_uin = new QLabel(i18n("Uin"), vbox11);
	e_uin = new QLineEdit(vbox11);
	grid->addMultiCellWidget(vbox11, 0, 1, 0, 0);

	QVBox *vbox12 = new QVBox(this);
	QLabel *l_group = new QLabel(i18n("Group"), vbox12);
	e_group = new QLineEdit(vbox12);
	grid->addMultiCellWidget(vbox12, 0, 1, 1, 1);

	QVBox *vbox21 = new QVBox(this);
	QLabel *l_nickname = new QLabel(i18n("Nickname"), vbox21);
	e_nickname = new QLineEdit(vbox21);
	grid->addMultiCellWidget(vbox21, 2, 3, 0, 0);

	QVBox *vbox22 = new QVBox(this);
	QLabel *l_altnick = new QLabel(i18n("AltNick"), vbox22);
	e_altnick = new QLineEdit(vbox22);
	grid->addMultiCellWidget(vbox22, 2, 3, 1, 1);

	QVBox *vbox31 = new QVBox(this);
	QLabel *l_fname = new QLabel(i18n("Name"), vbox31);
	e_fname = new QLineEdit(vbox31);
	grid->addMultiCellWidget(vbox31, 4, 5, 0, 0);

	QVBox *vbox32 = new QVBox(this);
	QLabel *l_lname = new QLabel(i18n("Surname"), vbox32);
	e_lname = new QLineEdit(vbox32);
	grid->addMultiCellWidget(vbox32, 4, 5, 1, 1);

	QVBox *vbox41 = new QVBox(this);
	QLabel *l_tel = new QLabel(i18n("Cell #"), vbox41);
	e_tel = new QLineEdit(vbox41);
	grid->addMultiCellWidget(vbox41, 6, 7, 0, 0);

	QVBox *vbox42 = new QVBox(this);
	QLabel *l_email = new QLabel(i18n("Email"), vbox42);
	e_email = new QLineEdit(vbox42);
	grid->addMultiCellWidget(vbox42, 6, 7, 1, 1);

	QPushButton *b_commit = new QPushButton(i18n("&Add"), this);
	b_commit->setAccel(Key_Return);	
	grid->addWidget(b_commit, 8, 0);
	QPushButton *b_cancel = new QPushButton(i18n("&Cancel"), this);
	grid->addWidget(b_cancel, 8, 1);

	QObject::connect(e_nickname, SIGNAL(textChanged(const QString &)), e_altnick,
		SLOT(setText(const QString &)));
	QObject::connect(b_commit, SIGNAL(clicked()), this, SLOT(Add()));
	QObject::connect(b_cancel, SIGNAL(clicked()), this, SLOT(reject()));

	kdebug("Adduser::Adduser(): finished\n");
}

void Adduser::Add() {
	kdebug("Adduser::Add()\n");

	if (e_altnick->text().length()) {
		kadu->addUser(e_fname->text(), e_lname->text(), e_nickname->text(),
			e_altnick->text(), e_tel->text(), e_uin->text(),
			GG_STATUS_NOT_AVAIL, e_group->text(), "", e_email->text());
		close(true);
		}
	else
		QMessageBox::warning(this, i18n("Add user problem"),
			i18n("Altnick field cannot be empty."));

	kdebug("Adduser::Add(): finished\n");
}

