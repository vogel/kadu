#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include "contacts/contact-list.h"
#include "contacts/contact-set.h"
#include "gui/actions/action.h"
#include "gui/windows/main-window.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTreeWidget;
class QTreeWidgetItem;
class SearchService;

struct KADUAPI SearchResult
{
	QString Uin;
	QString First;
	QString Last;
	QString Nick;
	QString Born;
	QString City;
	QString FamilyName;
	QString FamilyCity;
	int Gender;
	///GaduStatus Stat;

	SearchResult();
	SearchResult(const SearchResult &);
	void setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
		const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status);
};

typedef QList<SearchResult> SearchResults;

struct KADUAPI SearchRecord
{
	int Seq;
	int FromUin;
	QString Uin;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString City;
	QString BirthYearFrom;
	QString BirthYearTo;
	int Gender;
	bool Active;
	bool IgnoreResults;

	SearchRecord();
	virtual ~SearchRecord();

	void reqUin(const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();
};

// TODO: a better name
class SearchActionsSlots : public QObject
{
	Q_OBJECT

public slots:
	void firstSearchActionCreated(Action *action);
	void nextResultsActionCreated(Action *action);
	void stopSearchActionCreated(Action *action);
	void clearResultsActionCreated(Action *action);
	void actionsFoundActionCreated(Action *action);

	void firstSearchActionActivated(QAction *sender, bool toggled);
	void nextResultsActionActivated(QAction *sender, bool toggled);
	void stopSearchActionActivated(QAction *sender, bool toggled);
	void clearResultsActionActivated(QAction *sender, bool toggled);
	void addFoundActionActivated(QAction *sender, bool toggled);
	void chatFoundActionActivated(QAction *sender, bool toggled);

};


class KADUAPI SearchWindow : public MainWindow
{
	Q_OBJECT

	friend class SearchActionsSlots;
	static SearchActionsSlots *searchActionsSlot;

	static ActionDescription *firstSearchAction;
	static ActionDescription *nextResultsAction;
	static ActionDescription *stopSearchAction;
	static ActionDescription *clearResultsAction;
	static ActionDescription *addFoundAction;
	static ActionDescription *chatFoundAction;

	SearchService *CurrentSearchService;

	QCheckBox *only_active;
	QLineEdit *e_uin;
	QLineEdit *e_name;
	QLineEdit *e_nick;
	QLineEdit *e_byrFrom;
	QLineEdit *e_byrTo;
	QLineEdit *e_surname;
	QComboBox *c_gender;
	QLineEdit *e_city;
	QTreeWidget *results;
	QLabel *progress;
	QRadioButton *r_uin;
	QRadioButton *r_pers;
	Contact CurrentContact;
	Account *CurrentAccount;
	uint32_t seq;
	ContactSet selectedUsers;

	SearchRecord *searchRecord;

	bool searchhidden;
	bool searching;
	bool workaround; // TODO: remove

	bool isPersonalDataEmpty() const;

	ContactSet selected();

	QTreeWidgetItem * selectedItem();

	void setActionState(ActionDescription *action, bool toogle);

private slots:
	void uinTyped(void);
	void personalDataTyped(void);
	void byrFromDataTyped(void);
	void persClicked();
	void uinClicked();
	void updateInfoClicked();
	void selectionChanged();

protected:
	/**
		\fn void closeEvent(QCloseEvent * e)
		Obs�uguje zdarzenie zamkni�cia okna wyszukiwania w katalogu publicznym.
		\param e wska�nik do obiektu opisuj�cego zdarzenie zamkni�cie okna.
	**/
	virtual void closeEvent(QCloseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:
	/**
		\fn SearchWindow(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0)
		Standardowy konstruktor.
		\param parent rodzic kontrolki. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
		\param whoisSearchUin warto�� logiczna informuj�ca o tym, czy wst�pnie ma by� wybrane
		wyszukiwanie po numerze UIN (1) czy po danych osobowych (0). Domy�lnie 0.
	**/
	SearchWindow(QWidget *parent=0, Contact contact = Contact::null);
	~SearchWindow(void);

	static void createDefaultToolbars(QDomElement parentConfig);

	static void initModule();
	static void closeModule();

	virtual bool supportsActionType(ActionDescription::ActionType type) { return type & ActionDescription::TypeSearch; }
	//virtual UserBox * userBox() { return 0; }
	//virtual UserListElements userListElements() { return UserListElements(); }
	//virtual ChatWidget * chatWidget() { return 0; }

	virtual ContactsListView* contactsListView() { return 0; }
	virtual ContactSet contacts() { return ContactSet(); }
	virtual Chat* chat() { return 0; }


	/**
		\fn void nextSearch()
		Kontynuuje wyszukowanie kolejnych kontakt�w, a wyniki dodaje do bierz�cych.
		Metoda ta wywo�ywana jest przy wci�ni�ciu przycisku "Nast�pne wyniki".
	**/
	void nextSearch();

	/**
		\fn void stopSearch(void)
		Zatrzymuje aktualne wyszukiwanie. Je�li w p��niejszym czasie zwr�cone
		zostan� jakie� wyniki, b�d� one zignorowane.
	**/
	void stopSearch();

	void clearResults();

	void addFound();
	void chatFound();

public slots:
	/**
		\fn void newSearchResults(SearchResults& searchResults, int seq, int fromUin)
		Interpretuje uzyskane wyniki wyszukiwania i dodaje je do listy wynik�w.
		Metoda ta jest wywo�ywana, gdy serwer Gadu-Gadu odpowie na zapytanie do katalogu publicznego.
		\param searchResults lista struktur opisuj�cych wyniki wyszukiwania.
		\param seq unikalny identyfikator zapytania do katalogu publicznego.
		\param fromUin numer UIN, od kt�rego rozpocz�to wyszukiwanie (jest r��ny dla kolejnych
		wywo�a� - najpierw SearchWindow::firstSearch, a potem kolejne SearchWindow::nextSearch).
	**/
	void newSearchResults(ContactList contacts);

	/**
		\fn void firstSearch()
		Czy�ci list� wynik�w, a nast�pnie wyszukuje w katalogu publicznym wg.
		podanych w oknie danych. Wy�wietla tylko ograniczon� ich liczb�, ze wzgl�du
		na dzia�anie protoko�u Gadu-Gadu. Metoda ta wywo�ywana jest przy wci�ni�ciu
		przycisku "Szukaj". Aby uzyska� kolejne wyniki i doda� je do
		bierz�cych, nale�y dokona� wt�rnego zapytania metod� SearchWindow::nextSearch.
	**/
	void firstSearch();
};

#endif

