/***************************************************************************
                          adduser.cpp  -  description
                             -------------------
    begin                : Sat Nov 3 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

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
#include <klocale.h>

//
#include "kadu.h"
#include "adduser.h"
//

Adduser::Adduser(QDialog* parent, const char *name) {

resize(400,150);
setCaption(i18n("Add user"));

QLabel * descr = new QLabel(this);
descr->setText(i18n("ATTENTION! As of version 0.3.1, Kadu identifies users by the ALTNICK,\n"
		"not Nickname field. This is similar to EKG, GG and GNU Gadu"));

QLabel * l_nickname = new QLabel(this); l_nickname->setText(i18n("AltNick"));
e_nickname = new QLineEdit(this);

QLabel * l_uin = new QLabel(this); l_uin->setText(i18n("Uin"));
e_uin = new QLineEdit(this);

e_fname = new QLineEdit(this);
QLabel * l_fname = new QLabel(this); l_fname->setText(i18n("Name"));

e_lname = new QLineEdit(this);
QLabel * l_lname = new QLabel(this); l_lname->setText(i18n("Surname"));

e_altnick = new QLineEdit(this);
QLabel * l_altnick = new QLabel(this); l_altnick->setText(i18n("Nickname"));

e_tel = new QLineEdit(this);
QLabel * l_tel = new QLabel(this); l_tel->setText(i18n("Cell #"));

effect = new QLabel(this);

QPushButton * b_commit = new QPushButton(this);
b_commit->setText(i18n("&Add"));
b_commit->setAccel(Key_Return);	

QPushButton * b_cancel = new QPushButton(this);
b_cancel->setText(i18n("&Cancel"));

QObject::connect(e_nickname, SIGNAL( textChanged(const QString &) ), e_altnick, SLOT( setText(const QString &) ));
QObject::connect(b_commit, SIGNAL( clicked() ), this, SLOT( Add() ));
QObject::connect(b_cancel, SIGNAL( clicked() ), this, SLOT( reject() ));

QGridLayout * grid = new QGridLayout (this, 5, 5, 6, 6);
grid->addColSpacing(2,30);
grid->addMultiCellWidget(descr,0,0,0,4);

grid->addWidget(l_nickname,1,0);
grid->addWidget(e_nickname,1,1);

grid->addWidget(l_uin,1,3);
grid->addWidget(e_uin,1,4);

grid->addWidget(l_fname,2,0);
grid->addWidget(e_fname,2,1);

grid->addWidget(l_lname,2,3);
grid->addWidget(e_lname,2,4);

grid->addWidget(l_altnick,3,0);
grid->addWidget(e_altnick,3,1);

grid->addWidget(l_tel,3,3);
grid->addWidget(e_tel,3,4);

grid->addWidget(b_commit,4,4);
grid->addWidget(b_cancel,4,3);
grid->addMultiCellWidget(effect,4,4,0,1);

}

void Adduser::Add() {
	kadu->addUser(e_fname->text(),e_lname->text(),e_nickname->text(),
		e_altnick->text(),e_tel->text(),e_uin->text(),
		GG_STATUS_NOT_AVAIL,"","");

	close(true);
}

#include "adduser.moc"
