/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QtCore/qdatetime.h>
#include <QtGui/qdialog.h>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/qtreewidget.h>
#include <QtCore/QList>
#include <QtGui/QMainWindow>
#include <QtGui/QCloseEvent>

#include "gui/actions/action-description.h"
#include "chat/message/message-render-info.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/windows/main-window.h"
#include "gui/windows/main-configuration-window.h"

#include "history-search-dialog.h"
#include "../../history.h"
#include "../../history_exports.h"

/*!
\struct HistorySearchParameters
\brief Parametry przeszukiwania historii
*/
struct HistorySearchParameters {
	QDate fromDate; /*!< data graniczna od kt�rej ma wyszukiwa� */
	QDate toDate; /*!< data graniczna do kt�rej ma wyszukiwa� */
	QString pattern; /*!< ci�g znak�w do wyszukania */
	HistoryEntryType currentType; /*!< troch� protetyczne okre�lenie czy szukamy w chatach, statusach czy smsach, zale�nie w kt�rej ga��zi g��wnej le�y zaznaczona pozycja w momencie u�ycia wyszukiwania. */
	bool isRegExp;  /*!< Czy szukamy za pomoc� wyra�enia regularnego. */
	bool isCaseSensv;  /*!< Czy szukanie uwzgl�dnia wielko�� znak�w. */
	bool wholeWordsSearch;  /*!< Czy szukamy jedynie ca�ych s��w. */
	int resultsShowMode;  /*!< Tryb wy�wietlania wynik�w: 0 - pojedyncze wiadomo�ci, 1 - w�tek do ko�ca, 2 - ca�y w�tek. */
};

/*!
\struct HistorySearchDetailsItem
\brief Kontener danych potrzebnych do odtworzenia listy dat z zapisanego wyniku wyszukiwania w widoku szczeg��owym.
*/
class HISTORYAPI HistorySearchDetailsItem
{
	public:
		HistorySearchDetailsItem(QString altNick,  QString title, QDate date, int length);
		QString altNick;  /*!< Ksywa kontaktu na potrzeby pozycji na li�cie widoku dat. */
		QDate date;  /*!< Data, dla kt�rej znaleziono wyst�pienia wzorca w wyszukiwaniu. */
		QString title;  /*!< Skr�t z pierwszej wiadomo�ci dla danej daty. */
		int length;  /*!< Ilo�� wiadomo�ci, w tym przypadku z wyst�pieniem wzorca. */
};

/*!
\struct HistorySearchResult
\brief Reprezentuje wyniki pojedynczego wyszukiwania.
*/
class HISTORYAPI HistorySearchResult
{
	public:
		HistorySearchResult();
		ContactList users;  /*!< Lista u�ytkownik�w, kt�rych histori� przeszukano. */
		HistoryEntryType currentType;  /*!< Troch� protetyczne okre�lenie czy szukali�my w chatach, statusach czy smsach */
		//QString itemLabel; 
		QList<HistorySearchDetailsItem> detailsItems;  /*!< Zachowana lista dat, w kt�rych znaleziono wyst�pienia frazy. */
		QString pattern;  /*!< Poszukiwany wzorzec. */
		int resultsShowMode;  /*!< Tryb wy�wietlania wynik�w. */
};

/*!
\class DetailsListViewText
\brief Lista widoku szczeg��owego wg. dat rozm�w w oknie przegl�dania historii
*/
class DetailsListItem : public QTreeWidgetItem
{
	private:
		QDate Date; /*!< Data, z kt�rej rozmowy reprezentuje dany element. */
		Chat *CurrentChat;  /*!< Lista u�ytkownik�w, do kt�rych nale�y dana data - czasem si� przyda, gdy nie mo�na zachowa� zaznaczenia na g��wnej li�cie. */
	public:
		/**
       		Standardowy konstruktor dla g��wnego elementu listy szczeg��owej.
		@param parent nadrz�dny treewidget
		@param title tytu� (fragment wiadomo�ci z danej daty)
		@param date data, dla kt�rej rozmowy reprezentuje dany element
		@param lenght ilo�� wiadomo�ci dla danej daty
		@param uids lista u�ytkownik�w, dla kt�rej pobrano dat� - jak ju� wspomniano, czasem si� przydaje.
		*/
		DetailsListItem(QTreeWidget* parent, Chat *chat, QDate date);
		/**
       		Zwraca dat� reprezentowan� przez dany element.
		@return Data przypisana do danego elementu.
		*/
		/**
       		Zwraca list� u�ytkownik�w reprezentowan� przez dany element.
		@return Lista u�ytkownik�w przypisana do danego elementu.
		*/
		Chat * chat() const { return CurrentChat; };
		QDate date() const { return Date; };
		QString prepareAltnick();
		QString prepareTitle();
		QString prepareLength();
};

/*!
\class HistoryMainWidget
\brief G��wny widget okna przegl�dania historii
*/
class HistoryMainWidget : public MainWindow
{
	Q_OBJECT
		ActionDescription *historySearchActionDescription; /*!< Akcja otwieraj�ca okno wyszukiwania. */
		ActionDescription *historyNextResultsActionDescription; /*!< Akcja przewijaj�ca okno wiadomo�ci do nast�pnego wyst�pienia wyszukiwanego wyra�enia. */
		ActionDescription *historyPrevResultsActionDescription; /*!< Akcja przewijaj�ca okno wiadomo�ci do poprzedniego wyst�pienia wyszukiwanego wyra�enia. */
		QTreeWidget* DetailsListView; /*!< Lista szczeg��owa rozm�w wg. dat. */
		ChatMessagesView* ContentBrowser; /*!< Okno przegl�dania rozm�w. */
		QDockWidget *dock;  /*!< Dokowany widget wyszukiwania w bie��cej zawarto�ci okna wiadomo�ci. */
		QLineEdit *quickSearchPhraseEdit;  /*!< LineEdit w widgecie wyszukiwania "na �ywcowo". */
	private slots:
		/**
       		Slot wyszukuj�cy na bie��co tekst podany w widgecie szybkiego wyszukiwania.
		*/
		void quickSearchPhraseTyped(const QString &text);

	public:
		/**
       		Konstruktor g��wnego elementu okna przegl�dania historii.
		@param parent widget nadrz�dny
		@param window wska�nik okna przegl�dania historii
		*/
		HistoryMainWidget(QWidget *parent, QWidget *window);
		virtual ~HistoryMainWidget();
		/**
       		Zwraca wska�nik do listy widoku szczeg��owego.
		@return Wska�nik do listy widoku szczeg��owego.
		*/
		virtual QTreeWidget* getDetailsListView() { return DetailsListView; };
		/**
       		Zwraca wska�nik do okna przegl�dania rozm�w.
		@return Wska�nik do okna przegl�dania rozm�w.
		*/
		virtual ChatMessagesView* getContentBrowser() { return ContentBrowser; };
		/**
       		Sprawdza, czy toolbary obiektu tej klasy wspieraj� dany typ akcji.
		@param type typ akcji.
		*/
		virtual bool supportsActionType(ActionDescription::ActionType type);
		virtual ContactSet contacts() { return ContactSet(); };
		virtual ChatWidget * getChatWidget() { return 0; };
		virtual QDockWidget * getDockWidget() { return dock; };
		virtual ContactsListWidget* contactsListWidget() { return 0; } 

};

class HistoryChatsModel;

/*!
\class HistoryDlg
\author Juzef
\brief Okno przegl�dania historii.
*/
class HistoryWindow : public QWidget
{
	Q_OBJECT

	QTreeView *ChatsTree;
	HistoryChatsModel *ChatsModel;

		HistoryMainWidget *main; /*!< G��wny widget okna z toolbarami i innymi bajerami. */
		QMenu *MainPopupMenu; /*!< Menu kontekstowe g��wnej listy u�ytkownik�w. */
		QMenu *DetailsPopupMenu; /*!< Menu kontekstowe widoku dat. */
		QList<HistorySearchResult> previousSearchResults;  /*!< Wyniki poprzednich wyszukiwa�. */

		HistorySearchParameters searchParameters; /*!< Parametry ostatniego wyszukiwania. */
 		//QList<ContactList> uid_groups; /*!< Lista grup u�ytkownik�w, kt�rych rozmowy zapisano w historii. */
 		//QList<ContactList> status_uid_groups; /*!< Lista grup u�ytkownik�w, kt�rych zmiany status�w zapisano w historii. */
		bool closeDemand; /*!< Okre�la, czy by�o ��danie zamkni�cia okna w trakcie wyszukiwania. */
		bool isSearchInProgress; /*!< Okre�la, czy trwa aktualnie wyszukiwanie. */
		bool inSearchMode;  /*!< Okre�la, czy trwa przegl�danie wynik�w bie��cego lub wcze�niejszego wyszukiwania, czyli czy nale�y zaznacza� wyst�pienia frazy w oknie wiadomo�ci. */
		HistorySearchDialog *advSearchWindow; /*!< Okno wyszukiwania. */
// 		QList<int> anchors; /*!< Namiary na wyst�pienia poszukiwanej frazy w ci�gu rozm�w. */
// 		QList<int>::Iterator idxIt; 
		/**
       		Uruchamia przeszukiwanie historii wedle bie��cych kryteri�w @see searchParameters.
		*/
		void searchHistory();
		/**
       		Od�wie�a zawarto�� okna przegl�dania historii.
		*/
		void globalRefresh();
		/**
       		Od�wie�a zawarto�� ga��zi z wynikami wyszukiwa�.
		*/
		void searchBranchRefresh();
		ContactSet selectedUsers; /*!< U�ytkownicy, dla kt�rych wyst�pi�o ��danie otwarcia okna historii. */
		int maxLen;  /*!< Max. d�ugo�� zajawki wiadomo�ci w widoku dat. */
private slots:
	void chatActivated(const QModelIndex &index);
		
		/**
       		Slot od�wie�aj�cy zawarto�� okna przegl�dania rozm�w wg. zmiany zaznaczenia na li�cie szczeg��owej.
		@param item Element listy wybrany przez u�ytkownika.
		*/
		void detailsListItemClicked(QTreeWidgetItem *item, int column);
		/**
       		Slot otwieraj�cy okno wyszukiwania.
		*/
		void searchActionActivated(QAction* sender, bool toggled);
		/**
       		Slot przewijaj�cy zawarto�� okna przegl�dania rozm�w do nast�pnego wyst�pienia znalezionego wyra�enia.
		*/
		void searchNextActActivated(QAction* sender, bool toggled);
		/**
       		Slot przewijaj�cy zawarto�� okna przegl�dania rozm�w do poprzedniego wyst�pienia znalezionego wyra�enia.
		*/
		void searchPrevActActivated(QAction* sender, bool toggled);
		/**
       		Slot wy�wietlaj�cy menu kontekstowe dla g�. listy u�ytkownik�w.
		*/
		void showMainPopupMenu(const QPoint & pos);
		/**
       		Slot wy�wietlaj�cy menu kontekstowe dla listy widoku dat.
		*/
		void showDetailsPopupMenu(const QPoint & pos);
		/**
       		Slot pozwalaj�cy otworzy� okno rozmowy z poziomu g�. listy u�ytkownik�w.
		*/
		void openChat();
		/**
       		Otwiera okno wyszukiwania w katalogu dla wybranego kontaktu z poziomu g�. listy u�ytkownik�w.
		*/
		void lookupUserInfo();
		/**
       		Slot pozwalaj�cy usun�� zapisy w historii danego kontaktu z poziomu g�. listy u�ytkownik�w.
		*/
		void removeHistoryEntriesPerUser();
		/**
       		Usuwa zapisy w historii danego kontaktu dla wybranej daty.
		*/
		void removeHistoryEntriesPerDate();
	protected:
		/**
       		Reimplementacja obs�ugi zdarzenia zamkni�cia okna.
		*/
		void closeEvent(QCloseEvent *e);
		/**
       		Reimplementacja obs�ugi zdarzenia naci�ni�cia klawisza - potrzebna dla wyszukiwania pod Ctrl+F.
		*/
		void keyPressEvent(QKeyEvent *e);

	public:
		HistoryWindow();
		~HistoryWindow();
		/**
       		Zwraca struktur� z aktualnymi parametrami wyszukiwania.
		*/
		virtual HistorySearchParameters getSearchParameters() const { return searchParameters; };
		/**
       		Zapisuje struktur� z aktualnymi parametrami wyszukiwania.
		@param params Struktura do zapisania.
		*/
		void setSearchParameters(HistorySearchParameters& params);
		/**
       		Wy�wietla okno dialogowe historii.
		@param users Lista kontakt�w z aktywnego userboxa.
		*/
		void show(ContactSet contacts);
		virtual QMenu * getMainPopupMenu() { return MainPopupMenu;};
	
		virtual QList<HistorySearchResult> getPreviousSearchResults() const { return previousSearchResults; };
};

#endif // HISTORY_WINDOW_H
