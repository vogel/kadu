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

#include <qtabdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>

class UserInfo : public QTabDialog {
	Q_OBJECT
	public:
		UserInfo(const QString &, QDialog* parent, const QString &altnick);

	private:
		unsigned int this_index;
		QLineEdit *e_firstname;
		QLineEdit *e_lastname;
		QLineEdit *e_nickname;
		QLineEdit *e_altnick;
		QLineEdit *e_mobile;
		QLineEdit *e_uin;
		QLineEdit *e_addr;
		QLineEdit *e_group;		
		QCheckBox *c_blocking;
		QCheckBox *c_offtouser;
		QCheckBox *c_notify;

	protected:
		void setupTab1();
		void setupTab2();

	private slots:
		void writeUserlist();
};

#endif
