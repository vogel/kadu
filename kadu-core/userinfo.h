#ifndef KADU_USERINFO_H
#define KADU_USERINFO_H

#include <qhbox.h>
#include <qvaluelist.h>

#include "userlistelement.h"
#include "misc.h"

class LayoutHelper;
class QCheckBox;
class QDns;
class QLineEdit;
class QPushButton;
class QScrollView;
class QTabWidget;
class QVBox;
class QVGroupBox;

/**
	Klasa ta reprezentuje okno, z którym mamy do czynienia podczas dodawania nowego kontaktu,
	lub podczas edycji danych istniej±cego ju¿ kontaktu.
	\class UserInfo
	\brief Okno dialogowe pokazuj±ce informacje o wybranym kontakcie
**/
class UserInfo : public QHBox
{
	Q_OBJECT
	public:
		/**
			\var static CreateNotifier createNotifier
			Statyczny obiekt wysy³aj±cy sygna³ CreateNotifier::objectCreated
			je¶li powstanie nowa instancja okienka.
		**/
		static CreateNotifier createNotifier;

		/**
			\fn UserInfo(const QString &altnick, bool addUser = false, QDialog* parent=0, const char *name=0)
			Konstruktor tworz±cy okno dialogowe z informacjami o danym kontakcie.
			\param altnick pseudonim kontaktu wy¶wietlany na li¶cie.
			\param addUser TRUE gdy dodajemy kontakt, FALSE gdy edytujemy dane istniej±cego. Domy¶lnie FALSE.
			\param parent wska¼nik do obiektu stanowi±cego rodzica. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
		**/
		UserInfo(UserListElement user, QWidget* parent = 0, const char *name = 0);
		~UserInfo();

		QTabWidget *tabs() {return tw_main;}
		UserListElement user() const {return User;}
		static bool acceptableGroupName(const QString &groupName);

	private:
		UserListElement User;
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
		QScrollView* scrollView;

		QValueList<QCheckBox *> groups;
		QValueList<QCheckBox *> hiddenCheckBoxes;
		QLineEdit *newGroup;
		QVBox *groupsBox;
		LayoutHelper *layoutHelper;

		void keyPressEvent(QKeyEvent *);

	protected:
		/**
			\fn void setupTab1()
			Tworzy pierwsz± zak³adkê.
		**/
		void setupTab1();

		/**
			\fn void setupTab2()
			Tworzy drug± zak³adkê.
		**/
		void setupTab2();

		/**
			\fn void setupTab3()
			Tworzy trzeci± zak³adkê.
		**/
		void setupTab3();

		virtual void resizeEvent(QResizeEvent *);

	public slots:
		/**
			\fn void updateUserlist()
			Wci¶niêcie przycisku Dodaj/Uaktualnij
		**/
		void updateUserlist();

	private slots:
		/**
			\fn void resultsReady()
			Ustawia warto¶æ pola DNS na znalezion± nazwê domeny.
		**/
		void resultsReady();

		/**
			\fn void newGroupClicked()
			wci¶niêto przycisk tworzenia nowej grupy
		**/
		void newGroupClicked();

		/**
			\fn void scrollToBottom();
			po dodaniu nowej grupy przewija listê grup aby dodana by³a widoczna
		**/
		void scrollToBottom();
	signals:
		void updateClicked(UserInfo *);
};

#endif
