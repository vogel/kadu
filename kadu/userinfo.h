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
#include <qdns.h>

#include "userlist.h"

class UserInfo : public QTabDialog {
	Q_OBJECT
	public:
		UserInfo(const QString &, QDialog* parent, const QString &altnick);
		~UserInfo();

	private:
		UserListElement *puser;
		QLineEdit *e_firstname;
		QLineEdit *e_lastname;
		QLineEdit *e_nickname;
		QLineEdit *e_altnick;
		QLineEdit *e_mobile;
		QLineEdit *e_uin;
		QLineEdit *e_addr;
		QLineEdit *e_ver;
		QLineEdit *e_group;		
		QLineEdit *e_dnsname;
		QCheckBox *c_blocking;
		QCheckBox *c_offtouser;
		QCheckBox *c_notify;
		QDns *dns;

	protected:
		void setupTab1();
		void setupTab2();

	public slots:
		void resultsReady();

	private slots:
		void writeUserlist();
};

#endif
