/***************************************************************************
                          register.h  -  description
                             -------------------
    begin                : Sat Dec 8 2001
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

#ifndef REGISTER_H
#define REGISTER_H

#include <qdialog.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include "../libgadu/lib/libgadu.h"

class Register : public QDialog {
	Q_OBJECT

	public:
		Register(QDialog* parent=0, const char *name=0);

	private:
		QTimer * sokiet;
		struct gg_http *h;
		struct gg_pubdir *p;
		QLineEdit * pwd, * pwd2, * mail;
		QLabel * status;
		uin_t uin;
		QCheckBox * updateconfig;
	  	void ask();	

	private slots:
		void doRegister();
		void watchSocket();
};

#endif
