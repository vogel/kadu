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

struct HistorySearchParameters {
	QDate fromDate;
	QDate toDate;
	QString pattern;
	HistoryEntryType currentType;
	bool isRegExp;
	bool isCaseSensv;
	bool wholeWordsSearch;
	int resultsShowMode;
};

class HISTORYAPI HistorySearchDetailsItem
{
	public:
		HistorySearchDetailsItem(QString altNick,  QString title, QDate date, int length);
		QString altNick;
		QDate date;
		QString title;
		int length;
};

class HISTORYAPI HistorySearchResult
{
	public:
		HistorySearchResult();
		ContactList users;
		HistoryEntryType currentType;
		QList<HistorySearchDetailsItem> detailsItems;
		QString pattern;
		int resultsShowMode;
};

class HistoryMainWidget : public MainWindow
{
	Q_OBJECT
		ActionDescription *historySearchActionDescription;
		ActionDescription *historyNextResultsActionDescription;
		ActionDescription *historyPrevResultsActionDescription;
		QTreeView *DetailsListView;
		ChatMessagesView* ContentBrowser;
		QDockWidget *dock;
		QLineEdit *quickSearchPhraseEdit;
	private slots:
		void quickSearchPhraseTyped(const QString &text);

	public:
		HistoryMainWidget(QWidget *parent, QWidget *window);
		virtual ~HistoryMainWidget();

		virtual QTreeView * getDetailsListView() { return DetailsListView; };

		virtual ChatMessagesView* getContentBrowser() { return ContentBrowser; };

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
*/
class HistoryWindow : public QWidget
{
	Q_OBJECT

	QTreeView *ChatsTree;
	HistoryChatsModel *ChatsModel;

		HistoryMainWidget *main;
		QMenu *MainPopupMenu;
		QMenu *DetailsPopupMenu;
		QList<HistorySearchResult> previousSearchResults;

		HistorySearchParameters searchParameters;
		bool closeDemand;
		bool isSearchInProgress;
		bool inSearchMode;
		HistorySearchDialog *advSearchWindow;

		void searchHistory();
		void globalRefresh();
		void searchBranchRefresh();
		ContactSet selectedUsers;
		int maxLen;
private slots:
	void chatActivated(const QModelIndex &index);
	void dateActivated(const QModelIndex &index);

		void searchActionActivated(QAction* sender, bool toggled);
		void searchNextActActivated(QAction* sender, bool toggled);
		void searchPrevActActivated(QAction* sender, bool toggled);
		void showMainPopupMenu(const QPoint & pos);
		void showDetailsPopupMenu(const QPoint & pos);
		void openChat();
		void lookupUserInfo();
		void removeHistoryEntriesPerUser();
		void removeHistoryEntriesPerDate();
	protected:
		void closeEvent(QCloseEvent *e);
		void keyPressEvent(QKeyEvent *e);

	public:
		HistoryWindow();
		~HistoryWindow();

		virtual HistorySearchParameters getSearchParameters() const { return searchParameters; };
		void setSearchParameters(HistorySearchParameters& params);
		void show(ContactSet contacts);
		virtual QMenu * getMainPopupMenu() { return MainPopupMenu;};
	
		virtual QList<HistorySearchResult> getPreviousSearchResults() const { return previousSearchResults; };
};

#endif // HISTORY_WINDOW_H
