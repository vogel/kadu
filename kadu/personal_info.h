#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qhbox.h>
#include "gadu.h"

class QComboBox;
class QLineEdit;
class QPushButton;

/**
	\class PersonalInfoDialog
	\brief W³asne dane w katalogu publicznym
	Dialog umo¿liwiaj±cy zarz±dzanie w³asnymi danymi osobowymi w katalogu
	publicznym.
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
		/**
			\fn PersonalInfoDialog(QDialog *parent=0, const char *name=0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
		**/
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);
		~PersonalInfoDialog();

	public slots:
		/**
			\fn void fillFields(SearchResults& searchResults, int seq, int)
			Metoda ta wype³nia pola danych osobowych w tym oknie wg. otrzymanych
			- aktualnych - danych z serwera.
			\param searchResults dane otrzymane z serwera.
			\param seq unikalny identyfikator zapytania do serwera Gadu-Gadu.
			Trzeci parametr w tym przypadku jest pomijany.
		**/
		void fillFields(SearchResults& searchResults, int seq, int);
};

#endif
