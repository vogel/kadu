#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <QtGui/QWidget>

#include "contacts/contact.h"

class QComboBox;
class QKeyEvent;
class QLineEdit;
class QPushButton;
class QResizeEvent;

class PersonalInfoService;
class Protocol;

/**
	\class PersonalInfoDialog
	\brief W�asne dane w katalogu publicznym
	Dialog umo�liwiaj�cy zarz�dzanie w�asnymi danymi osobowymi w katalogu
	publicznym.
**/
class PersonalInfoDialog : public QWidget
{
	Q_OBJECT

	enum DialogState
	{
		Ready,
		Reading,
		Writing
	};

	Protocol *CurrentProtocol;
	PersonalInfoService *CurrentService;

	QLineEdit *le_nickname;
	QLineEdit *le_name;
	QLineEdit *le_surname;
	QComboBox *cb_gender;
	QLineEdit *le_birthyear;
	QLineEdit *le_city;
	QLineEdit *le_familyname;
	QLineEdit *le_familycity;
	QPushButton *pb_save;
	DialogState State;

private slots:
	void saveButtonClicked();
	void keyPressEvent(QKeyEvent *);
	void reloadInfo();

	void personalInfoAvailable(Contact contact);
	void personalInfoUpdated(bool);

public:
	/**
		\fn PersonalInfoDialog(QWidget *parent=0, const char *name=0)
		Standardowy konstruktor.
		\param parent rodzic kontrolki. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
	**/
	PersonalInfoDialog(Protocol *protocol, QWidget *parent = 0);
	~PersonalInfoDialog();

};

#endif
