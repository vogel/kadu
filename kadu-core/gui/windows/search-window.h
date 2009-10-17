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
	void selectionChanged();

protected:

	virtual void closeEvent(QCloseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:

	SearchWindow(QWidget *parent=0, Contact contact = Contact::null);
	~SearchWindow(void);

	static void createDefaultToolbars(QDomElement parentConfig);

	static void initModule();
	static void closeModule();

	virtual bool supportsActionType(ActionDescription::ActionType type) { return type & ActionDescription::TypeSearch; }
	virtual ContactsListView* contactsListView() { return 0; }
	virtual ContactSet contacts() { return ContactSet(); }
	virtual Chat* chat() { return 0; }


	void nextSearch();
	void stopSearch();
	void clearResults();

	void addFound();
	void chatFound();

public slots:

	void newSearchResults(ContactList contacts);
	void firstSearch();
};

#endif

