#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qhbox.h>
#include "gadu.h"

class QComboBox;
class QLineEdit;
class QPushButton;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie w³asnymi danymi w katalogu
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
		enum DialogState { READY, READING, WRITING };
		DialogState State;
		SearchRecord *data;

	private slots:
		void saveButtonClicked();
		void keyPressEvent(QKeyEvent *);
		void reloadInfo();

	public:
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);
		~PersonalInfoDialog();

	public slots:
		void fillFields(SearchResults& searchResults, int seq, int);
};

#endif
