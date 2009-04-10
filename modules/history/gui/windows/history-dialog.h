#ifndef HISTORY_DLG_H
#define HISTORY_DLG_H

#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/qtreewidget.h>
#include <QtCore/QList>
#include <QtGui/QMainWindow>
#include <QtGui/QCloseEvent>

#include "action.h"
#include "kadu_main_window.h"
#include "gui/widgets/chat_messages_view.h"
#include "gui/widgets/contacts-list-widget.h"

#include "history-search-dialog.h"
#include "../../history.h"

/*!
\struct HistorySearchParameters
\brief Parametry przeszukiwania historii
*/
struct HistorySearchParameters {
	QDate fromDate; /*!< data graniczna od której ma wyszukiwaæ */
	QDate toDate; /*!< data graniczna do której ma wyszukiwaæ */
	QString pattern; /*!< ci±g znaków do wyszukania */
	HistoryEntryType currentType; /*!< trochê protetyczne okre¶lenie czy szukamy w chatach, statusach czy smsach, zale¿nie w której ga³êzi g³ównej le¿y zaznaczona pozycja w momencie u¿ycia wyszukiwania. */
	bool isRegExp;  /*!< Czy szukamy za pomoc± wyra¿enia regularnego. */
	bool isCaseSensv;  /*!< Czy szukanie uwzglêdnia wielko¶æ znaków. */
	bool wholeWordsSearch;  /*!< Czy szukamy jedynie ca³ych s³ów. */
	int resultsShowMode;  /*!< Tryb wy¶wietlania wyników: 0 - pojedyncze wiadomo¶ci, 1 - w±tek do koñca, 2 - ca³y w±tek. */
};

/*!
\struct HistorySearchDetailsItem
\brief Kontener danych potrzebnych do odtworzenia listy dat z zapisanego wyniku wyszukiwania w widoku szczegó³owym.
*/
struct HistorySearchDetailsItem
{
	HistorySearchDetailsItem(QString altNick,  QString title, QDate date, int length);
	QString altNick;  /*!< Ksywa kontaktu na potrzeby pozycji na li¶cie widoku dat. */
	QDate date;  /*!< Data, dla której znaleziono wyst±pienia wzorca w wyszukiwaniu. */
	QString title;  /*!< Skrót z pierwszej wiadomo¶ci dla danej daty. */
	int length;  /*!< Ilo¶æ wiadomo¶ci, w tym przypadku z wyst±pieniem wzorca. */
};

/*!
\struct HistorySearchResult
\brief Reprezentuje wyniki pojedynczego wyszukiwania.
*/
struct HistorySearchResult
{
	HistorySearchResult();
	ContactList users;  /*!< Lista u¿ytkowników, których historiê przeszukano. */
	HistoryEntryType currentType;  /*!< Trochê protetyczne okre¶lenie czy szukali¶my w chatach, statusach czy smsach */
	//QString itemLabel; 
	QList<HistorySearchDetailsItem> detailsItems;  /*!< Zachowana lista dat, w których znaleziono wyst±pienia frazy. */
	QString pattern;  /*!< Poszukiwany wzorzec. */
	int resultsShowMode;  /*!< Tryb wy¶wietlania wyników. */
};


/*!
\class MainListViewText
\brief Pojedyncza pozycja z g³ównej listy w oknie przegl±dania historii.
*/
class MainListViewText : public QTreeWidgetItem
{
	private:
		ContactList Uids; /*!< Lista u¿ytkowników reprezentowanych przez dan± pozycjê na li¶cie */
		/**
       		Tworzy tekst bêd±cy nazw± danego elementu na li¶cie.
		*/
		void prepareText();
 
	public:
		/**
       		Standardowy konstruktor dla g³ównego elementu listy g³ównej.
		@param parent nadrzêdny treewidget
		@param uids lista u¿ytkowników
		*/
		MainListViewText(QTreeWidget* parent, const ContactList& uids);
		/**
       		Standardowy konstruktor dla podrzêdnego elementu listy g³ównej.
		@param parent nadrzêdny element
		@param uids lista u¿ytkowników
		*/
		MainListViewText(QTreeWidgetItem* parent, const ContactList& uids);
		/**
       		Zwraca listê u¿ytkowników reprezentowan± przez dany element.
		@return Lista u¿ytkowników przypisana do danego elementu.
		*/
		virtual const ContactList& uidsList() const { return Uids; };
};
 
/*!
\class DetailsListViewText
\brief Lista widoku szczegó³owego wg. dat rozmów w oknie przegl±dania historii
*/
class DetailsListViewItem : public QTreeWidgetItem
{
	private:
		QDate Date; /*!< Data, z której rozmowy reprezentuje dany element. */
		ContactList Uids;  /*!< Lista u¿ytkowników, do których nale¿y dana data - czasem siê przyda, gdy nie mo¿na zachowaæ zaznaczenia na g³ównej li¶cie. */
	public:
		/**
       		Standardowy konstruktor dla g³ównego elementu listy szczegó³owej.
		@param parent nadrzêdny treewidget
		@param title tytu³ (fragment wiadomo¶ci z danej daty)
		@param date data, dla której rozmowy reprezentuje dany element
		@param lenght ilo¶æ wiadomo¶ci dla danej daty
		@param uids lista u¿ytkowników, dla której pobrano datê - jak ju¿ wspomniano, czasem siê przydaje.
		*/
		DetailsListViewItem(QTreeWidget* parent, QString contact, QString title, QDate date, QString lenght, const ContactList& uids = ContactList());
		/**
       		Zwraca datê reprezentowan± przez dany element.
		@return Data przypisana do danego elementu.
		*/
		QDate date() const;
		/**
       		Zwraca listê u¿ytkowników reprezentowan± przez dany element.
		@return Lista u¿ytkowników przypisana do danego elementu.
		*/
		virtual const ContactList& uidsList() const { return Uids; };
};

/*!
\class HistoryMainWidget
\brief G³ówny widget okna przegl±dania historii
*/
class HistoryMainWidget : public KaduMainWindow
{
	Q_OBJECT
		ActionDescription *historySearchActionDescription; /*!< Akcja otwieraj±ca okno wyszukiwania. */
		ActionDescription *historyNextResultsActionDescription; /*!< Akcja przewijaj±ca okno wiadomo¶ci do nastêpnego wyst±pienia wyszukiwanego wyra¿enia. */
		ActionDescription *historyPrevResultsActionDescription; /*!< Akcja przewijaj±ca okno wiadomo¶ci do poprzedniego wyst±pienia wyszukiwanego wyra¿enia. */
		QTreeWidget* DetailsListView; /*!< Lista szczegó³owa rozmów wg. dat. */
		ChatMessagesView* ContentBrowser; /*!< Okno przegl±dania rozmów. */
		QDockWidget *dock;  /*!< Dokowany widget wyszukiwania w bie¿±cej zawarto¶ci okna wiadomo¶ci. */
		QLineEdit *quickSearchPhraseEdit;  /*!< LineEdit w widgecie wyszukiwania "na ¿ywcowo". */
	private slots:
		/**
       		Slot wyszukuj±cy na bie¿±co tekst podany w widgecie szybkiego wyszukiwania.
		*/
		void quickSearchPhraseTyped(const QString &text);

	public:
		/**
       		Konstruktor g³ównego elementu okna przegl±dania historii.
		@param parent widget nadrzêdny
		@param window wska¼nik okna przegl±dania historii
		*/
		HistoryMainWidget(QWidget *parent, QWidget *window);
		virtual ~HistoryMainWidget();
		/**
       		Zwraca wska¼nik do listy widoku szczegó³owego.
		@return Wska¼nik do listy widoku szczegó³owego.
		*/
		virtual QTreeWidget* getDetailsListView() { return DetailsListView; };
		/**
       		Zwraca wska¼nik do okna przegl±dania rozmów.
		@return Wska¼nik do okna przegl±dania rozmów.
		*/
		virtual ChatMessagesView* getContentBrowser() { return ContentBrowser; };
		/**
       		Sprawdza, czy toolbary obiektu tej klasy wspieraj± dany typ akcji.
		@param type typ akcji.
		*/
		virtual bool supportsActionType(ActionDescription::ActionType type);
		virtual ContactList contacts() { return ContactList(); };
		virtual ChatWidget * getChatWidget() { return 0; };
		virtual QDockWidget * getDockWidget() { return dock; };
		virtual ContactsListWidget* contactsListWidget() { return 0; } 

};

/*!
\class HistoryDlg
\author Juzef
\brief Okno przegl±dania historii.
*/
class HistoryDlg : public QWidget
{
	Q_OBJECT		
		QTreeWidgetItem* statusItem, *chatsItem, *smsItem, *conferItem, *searchItem, *anonChatsItem/*, *anonStatusItem*/;
	private:
  		HistoryMainWidget *main; /*!< G³ówny widget okna z toolbarami i innymi bajerami. */
		QMenu *MainPopupMenu; /*!< Menu kontekstowe g³ównej listy u¿ytkowników. */
		QMenu *DetailsPopupMenu; /*!< Menu kontekstowe widoku dat. */
		QList<HistorySearchResult> previousSearchResults;  /*!< Wyniki poprzednich wyszukiwañ. */
 		QTreeWidget* MainListView; /*!< G³ówna lista u¿ytkowników. */
		HistorySearchParameters searchParameters; /*!< Parametry ostatniego wyszukiwania. */
 		//QList<ContactList> uid_groups; /*!< Lista grup u¿ytkowników, których rozmowy zapisano w historii. */
 		//QList<ContactList> status_uid_groups; /*!< Lista grup u¿ytkowników, których zmiany statusów zapisano w historii. */
		bool closeDemand; /*!< Okre¶la, czy by³o ¿±danie zamkniêcia okna w trakcie wyszukiwania. */
		bool isSearchInProgress; /*!< Okre¶la, czy trwa aktualnie wyszukiwanie. */
		bool inSearchMode;  /*!< Okre¶la, czy trwa przegl±danie wyników bie¿±cego lub wcze¶niejszego wyszukiwania, czyli czy nale¿y zaznaczaæ wyst±pienia frazy w oknie wiadomo¶ci. */
		HistorySearchDialog *advSearchWindow; /*!< Okno wyszukiwania. */
// 		QList<int> anchors; /*!< Namiary na wyst±pienia poszukiwanej frazy w ci±gu rozmów. */
// 		QList<int>::Iterator idxIt; 
		/**
       		Uruchamia przeszukiwanie historii wedle bie¿±cych kryteriów @see searchParameters.
		*/
		void searchHistory();
		/**
       		Od¶wie¿a zawarto¶æ okna przegl±dania historii.
		*/
		void globalRefresh();
		/**
       		Od¶wie¿a zawarto¶æ ga³êzi z wynikami wyszukiwañ.
		*/
		void searchBranchRefresh();
		ContactList selectedUsers; /*!< U¿ytkownicy, dla których wyst±pi³o ¿±danie otwarcia okna historii. */
		int maxLen;  /*!< Max. d³ugo¶æ zajawki wiadomo¶ci w widoku dat. */
	private slots:
		/**
       		Slot od¶wie¿aj±cy zawarto¶æ listy szczegó³owej wg. zmiany zaznaczenia na li¶cie g³ównej.
		@param item Element listy wybrany przez u¿ytkownika.
		*/
		void mainItemChanged(QTreeWidgetItem *item, int column);
		/**
       		Slot od¶wie¿aj±cy zawarto¶æ okna przegl±dania rozmów wg. zmiany zaznaczenia na li¶cie szczegó³owej.
		@param item Element listy wybrany przez u¿ytkownika.
		*/
		void detailsItemChanged(QTreeWidgetItem *item, int column);
		/**
       		Slot otwieraj±cy okno wyszukiwania.
		*/
		void searchActionActivated(QAction* sender, bool toggled);
		/**
       		Slot przewijaj±cy zawarto¶æ okna przegl±dania rozmów do nastêpnego wyst±pienia znalezionego wyra¿enia.
		*/
		void searchNextActActivated(QAction* sender, bool toggled);
		/**
       		Slot przewijaj±cy zawarto¶æ okna przegl±dania rozmów do poprzedniego wyst±pienia znalezionego wyra¿enia.
		*/
		void searchPrevActActivated(QAction* sender, bool toggled);
		/**
       		Slot wy¶wietlaj±cy menu kontekstowe dla g³. listy u¿ytkowników.
		*/
		void showMainPopupMenu(const QPoint & pos);
		/**
       		Slot wy¶wietlaj±cy menu kontekstowe dla listy widoku dat.
		*/
		void showDetailsPopupMenu(const QPoint & pos);
		/**
       		Slot pozwalaj±cy otworzyæ okno rozmowy z poziomu g³. listy u¿ytkowników.
		*/
		void openChat();
		/**
       		Otwiera okno wyszukiwania w katalogu dla wybranego kontaktu z poziomu g³. listy u¿ytkowników.
		*/
		void lookupUserInfo();
		/**
       		Slot pozwalaj±cy usun±æ zapisy w historii danego kontaktu z poziomu g³. listy u¿ytkowników.
		*/
		void removeHistoryEntriesPerUser();
		/**
       		Usuwa zapisy w historii danego kontaktu dla wybranej daty.
		*/
		void removeHistoryEntriesPerDate();
	protected:
		/**
       		Reimplementacja obs³ugi zdarzenia zamkniêcia okna.
		*/
		void closeEvent(QCloseEvent *e);
		/**
       		Reimplementacja obs³ugi zdarzenia naci¶niêcia klawisza - potrzebna dla wyszukiwania pod Ctrl+F.
		*/
		void keyPressEvent(QKeyEvent *e);

	public:
		HistoryDlg();
		~HistoryDlg();
		/**
       		Zwraca strukturê z aktualnymi parametrami wyszukiwania.
		*/
		virtual HistorySearchParameters getSearchParameters() const { return searchParameters; };
		/**
       		Zapisuje strukturê z aktualnymi parametrami wyszukiwania.
		@param params Struktura do zapisania.
		*/
		void setSearchParameters(HistorySearchParameters& params);
		/**
       		Wy¶wietla okno dialogowe historii.
		@param users Lista kontaktów z aktywnego userboxa.
		*/
		void show(ContactList contacts);
		virtual QMenu * getMainPopupMenu() { return MainPopupMenu;};
	
		virtual QList<HistorySearchResult> getPreviousSearchResults() const { return previousSearchResults; };
};

#endif
