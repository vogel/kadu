#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include "libgadu.h"

/**
	Dialog umożliwiający zarządzanie własnymi danymi w katalogu
	publicznym
**/
class PersonalInfoDialog : public QHBox
{
	Q_OBJECT

	private:
		QLineEdit* le_nickname;
		QLineEdit* le_name;
		QLineEdit* le_surname;
		QComboBox* cb_gender;
		QLineEdit* le_birthyear;
		QLineEdit* le_city;
		QLineEdit* le_familyname;
		QLineEdit* le_familycity;
		QPushButton *pb_save;
		enum DialogState {READY, READING, WRITTING};
		DialogState State;
		uint32_t seq;

	private slots:
		void saveButtonClicked();
		void keyPressEvent(QKeyEvent *);
		void reloadInfo();

	protected:
		virtual void closeEvent(QCloseEvent * e);

	public:
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);
		~PersonalInfoDialog();

	public slots:
		void fillFields(gg_pubdir50_t res);
};

#endif
