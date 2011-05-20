/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QtGui/QDateEdit>
#include <QtGui/QTreeWidget>

#include "buddies/buddy-set.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/windows/main-window.h"

#include "search/history-search-parameters.h"
#include "history.h"
#include "history_exports.h"

class BuddyNameFilter;
class BuddyStatusDatesModel;
class ChatDatesModel;
class ChatNameFilter;
class HistoryChatsModel;
class HistoryChatsModelProxy;
class HistoryTreeItem;
class SmsDatesModel;
class TimedStatus;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public MainWindow
{
	Q_OBJECT

	friend class History;

	static HistoryWindow *Instance;

	QTreeView *ChatsTree;
	QTreeView *DetailsListView;
	HistoryChatsModel *ChatsModel;
	HistoryChatsModelProxy *ChatsModelProxy;
	BuddyNameFilter *StatusBuddyNameFilter;
	ChatNameFilter *NameFilter;

	ChatDatesModel *MyChatDatesModel;
	BuddyStatusDatesModel *MyBuddyStatusDatesModel;
	SmsDatesModel *MySmsDatesModel;

	QMenu *DetailsPopupMenu;
	QLabel *FromDateLabel;
	QDateEdit *FromDate;
	QLabel *ToDateLabel;
	QDateEdit *ToDate;

	ChatMessagesView *ContentBrowser;
	QLineEdit *quickSearchPhraseEdit;

	ActionDescription *historySearchActionDescription;
	ActionDescription *historyNextResultsActionDescription;
	ActionDescription *historyPrevResultsActionDescription;

	HistorySearchParameters Search;

	explicit HistoryWindow(QWidget *parent = 0);

	void createGui();
	void createChatTree(QWidget *parent);
	void createFilterBar(QWidget *parent);
	void connectGui();

	void updateData();
	void selectChat(const Chat &chat);
	void selectStatusBuddy(const Buddy &buddy);
	void selectSmsRecipient(const QString &recipient);
	void selectHistoryItem(const HistoryTreeItem &item);

	void chatActivated(const Chat &chat);
	void statusBuddyActivated(const Buddy &buddy);
	void smsRecipientActivated(const QString &recipient);
	void treeItemActivated(const HistoryTreeItem &item);

	QList<Message> statusesToMessages(const QList<TimedStatus> &statuses);

private slots:
	void treeCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void dateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	void filterLineChanged(const QString &filterText);
	void searchTextChanged(const QString &searchText);
	void fromDateChanged(const QDate &date);
	void toDateChanged(const QDate &date);

	void showMainPopupMenu(const QPoint &pos);
	void showDetailsPopupMenu(const QPoint &pos);
	void dateFilteringEnabled(int state);

	void openChat();
	void clearChatHistory();
	void clearStatusHistory();
	void clearSmsHistory();
	void removeHistoryEntriesPerDate();

	void selectQueryText();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	virtual ~HistoryWindow();

	virtual QTreeView * detailsListView() { return DetailsListView; }

	virtual ChatMessagesView * contentBrowser() { return ContentBrowser; }

	virtual bool supportsActionType(ActionDescription::ActionType type);
	virtual ContactSet contacts();
	virtual BuddySet buddies();
	virtual Chat chat();
	virtual ChatWidget * chatWidget() { return 0; }
	virtual BuddiesListView * buddiesListView() { return 0; }
	virtual StatusContainer* statusContainer() { return 0; }
	virtual bool hasContactSelected() { return false; } // we can select only buddies here

	static void show(const Chat &chat);
};

class HistoryChatsModel;

#endif // HISTORY_WINDOW_H
