#ifndef USERINFO_H
#define USERINFO_H

#include <qtabdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qdns.h>
#include <qcombobox.h>

#include "userlist.h"

/**
	Dialog pokazuj±cy informacje o wybranym kontakcie
**/
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
		QComboBox *cb_group;
		QLineEdit *e_email;
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
