/***************************************************************************
                         userinfo.h  -  description
                             -------------------
    begin                : pon cze 24 23:24:52 CEST 2002
    copyright            : (C) 2002 by chilek
    email                : chilek@chilan.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef USERINFO_H
#define USERINFO_H

#include <qdialog.h>
#include <qlineedit.h>
#include "../libgadu/lib/libgadu.h"

class UserInfo : public QDialog {
    Q_OBJECT
    public:
	UserInfo(const QString &, QDialog* parent=0, unsigned int uin = 0);
    
    private:
	unsigned int this_index;
	QLineEdit* e_firstname;
	QLineEdit* e_lastname;
	QLineEdit* e_nickname;
	QLineEdit* e_altnick;
	QLineEdit* e_mobile;
	QLineEdit* e_uin;
	QLineEdit* e_addr;
	QLineEdit* e_group;		

    private slots:
	void writeUserlist();
};

#endif
