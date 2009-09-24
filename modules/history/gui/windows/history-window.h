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

#include <QtGui/QTreeWidget>

#include "gui/actions/action-description.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/windows/main-window.h"

#include "modules/history/search/history-search-parameters.h"
#include "modules/history/history.h"
#include "modules/history/history_exports.h"

class ChatNameFilter;
class HistoryChatsModel;
class HistoryChatsModelProxy;

/*!
\class HistoryDlg
\author Juzef, Vogel
*/
class HistoryWindow : public MainWindow
{
	Q_OBJECT

	QTreeView *ChatsTree;
	QTreeView *DetailsListView;
	HistoryChatsModel *ChatsModel;
	HistoryChatsModelProxy *ChatsModelProxy;
	ChatNameFilter *NameFilter;

	QMenu *MainPopupMenu;
	QMenu *DetailsPopupMenu;

	ChatMessagesView* ContentBrowser;
	QLineEdit *quickSearchPhraseEdit;

	ActionDescription *historySearchActionDescription;
	ActionDescription *historyNextResultsActionDescription;
	ActionDescription *historyPrevResultsActionDescription;

	HistorySearchParameters Search;

	void createGui();
	void createChatTree(QWidget *parent);
	void createFilterBar(QWidget *parent);
	void connectGui();

	void updateData();
	void selectChat(Chat *chat);

private slots:
	void chatActivated(const QModelIndex &index);
	void dateActivated(const QModelIndex &index);

	void filterLineChanged(const QString &filterText);
	void searchTextChanged(const QString &searchText);
	void fromDateChanged(const QDate &date);
	void toDateChanged(const QDate &date);

	void showMainPopupMenu(const QPoint &pos);
	void showDetailsPopupMenu(const QPoint &pos);

	void openChat();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	HistoryWindow(QWidget *parent = 0);
	virtual ~HistoryWindow();

	virtual QTreeView * detailsListView() { return DetailsListView; }

	virtual ChatMessagesView * contentBrowser() { return ContentBrowser; }

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ContactSet contacts() { return ContactSet(); }
	virtual ChatWidget * chatWidget() { return 0; }
	virtual ContactsListWidget * contactsListWidget() { return 0; }
	virtual Chat * chat() { return 0; }

	void show(Chat *chat);

};

class HistoryChatsModel;

#endif // HISTORY_WINDOW_H
