#ifndef USERINFO_H
#define USERINFO_H

#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qdns.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qvgroupbox.h>

#include "userlist.h"
#include "misc.h"

/**
	Dialog pokazuj±cy informacje o wybranym kontakcie
**/
class UserInfo : public QHBox
{
	Q_OBJECT
	public:
		/**
			statyczny obiekt wysylajacy sygnal objectCreated
			jesli powstanie nowa instancja okienka.
		**/
		static CreateNotifier createNotifier;
		/**
			konstruktor
		**/
		UserInfo(const QString &altnick, bool addUser = false, QDialog* parent=0, const char *name=0);
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
		QLineEdit *e_email;
		QLineEdit *e_dnsname;
		QCheckBox *c_blocking;
		QCheckBox *c_offtouser;
		QCheckBox *c_notify;
		QPushButton *pb_addapply;
		QTabWidget *tw_main;
		QVGroupBox *vgb_general;
		QDns *dns;
		
		QValueList<QCheckBox *> groups;
		QValueList<QCheckBox *> hiddenCheckBoxes;
		QLineEdit *newGroup;
		QVBox *groupsBox;
		
		bool addUser;
		void addNewUser(UserListElement& e);
		void changeUserData(UserListElement& e);
		void keyPressEvent(QKeyEvent *);

	protected:
		void setupTab1();
		void setupTab2();
		void setupTab3();

	public slots:
		void resultsReady();
		void setUserInfo(UserListElement &ule);

	private slots:
		void updateUserlist();
		void newGroupClicked();
};

#endif
