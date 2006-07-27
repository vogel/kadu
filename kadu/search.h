#ifndef KADU_SEARCH_H
#define KADU_SEARCH_H

#include <qdialog.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include "gadu.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListViewItem;
class QPushButton;
class QRadioButton;

/**
	Klasa ta reprezentuje okno dialogowe wyszukiwania w katalogu publicznym.
	\brief Wyszukiwanie w katalogu publicznym.
	\class SearchDialog
**/
class SearchDialog : public QDialog {
	Q_OBJECT
	public:
		/**
			\fn SearchDialog(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki. Domy¶lnie 0.
			\param name nazwa kontrolki. Domy¶lnie 0.
			\param whoisSearchUin warto¶æ logiczna informuj±ca o tym, czy wstêpnie ma byæ wybrane
				wyszukiwanie po numerze UIN (1) czy po danych osobowych (0). Domy¶lnie 0.
		**/
		SearchDialog(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0);
		~SearchDialog(void);
		static void initModule();

	private:
		QCheckBox *only_active;
		QLineEdit *e_uin;
		QLineEdit *e_name;
		QLineEdit *e_nick;
		QLineEdit *e_byrFrom;
		QLineEdit *e_byrTo;
		QLineEdit *e_surname;
		QComboBox *c_gender;
		QLineEdit *e_city;
		QListView *results;
		QLabel *progress;
		QRadioButton *r_uin;
		QRadioButton *r_pers;
		UinType _whoisSearchUin;
		uint32_t seq;

		SearchRecord *searchRecord;

		bool searchhidden;

		bool isPersonalDataEmpty() const;

	public slots:

		void selectedUsersNeeded(const UserGroup*& user_group);

		/**
			\fn void firstSearch(void)
			Czy¶ci listê wyników, a nastêpnie wyszukuje w katalogu publicznym wg.
			podanych w oknie danych. Wy¶wietla tylko ograniczon± ich liczbê, ze wzglêdu
			na dzia³anie protoko³u Gadu-Gadu. Metoda ta wywo³ywana jest przy wci¶niêciu
			przycisku "Szukaj". Aby uzyskaæ kolejne wyniki i dodaæ je do
			bierz±cych, nale¿y dokonaæ wtórnego zapytania metod± SearchDialog::nextSearch.
		**/
		void firstSearch(void);

		/**
			\fn void nextSearch(void)
			Kontynuuje wyszukowanie kolejnych kontaktów, a wyniki dodaje do bierz±cych.
			Metoda ta wywo³ywana jest przy wci¶niêciu przycisku "Nastêpne wyniki".
		**/
		void nextSearch(void);

		/**
			\fn void newSearchResults(SearchResults& searchResults, int seq, int fromUin)
			Interpretuje uzyskane wyniki wyszukiwania i dodaje je do listy wyników.
			Metoda ta jest wywo³ywana, gdy serwer Gadu-Gadu odpowie na zapytanie do katalogu publicznego.
			\param searchResults lista struktur opisuj±cych wyniki wyszukiwania.
			\param seq unikalny identyfikator zapytania do katalogu publicznego.
			\param fromUin numer UIN, od którego rozpoczêto wyszukiwanie (jest ró¿ny dla kolejnych
			wywo³añ - najpierw SearchDialog::firstSearch, a potem kolejne SearchDialog::nextSearch).
		**/
		void newSearchResults(SearchResults& searchResults, int seq, int fromUin);

	private slots:
		void clearResults(void);
		void addSearchedActionActivated(const UserGroup*);
		void uinTyped(void);
		void personalDataTyped(void);
		void byrFromDataTyped(void);
		void persClicked();
		void uinClicked();
		void updateInfoClicked();

	protected:
		/**
			\fn void closeEvent(QCloseEvent * e)
			Obs³uguje zdarzenie zamkniêcia okna wyszukiwania w katalogu publicznym.
			\param e wska¼nik do obiektu opisuj±cego zdarzenie zamkniêcie okna.
		**/
		virtual void closeEvent(QCloseEvent *e);
		virtual void resizeEvent(QResizeEvent *e);
};

#endif

