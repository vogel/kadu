/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QMainWindow>
#include <QtGui/QTreeWidget>

#include "buddies/buddy-set.h"
#include "gui/widgets/chat-messages-view.h"
#include "talkable/talkable.h"

#include "history.h"
#include "history_exports.h"

class QStandardItemModel;

class BuddyListModel;
class BuddyStatusDatesModel;
class ChatDatesModel;
class ChatsListModel;
class HistoryTreeItem;
class ModelChain;
class NameTalkableFilter;
class SmsDatesModel;
class TalkableTreeView;
class TimedStatus;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public QMainWindow
{
	Q_OBJECT

	friend class History;

	static HistoryWindow *Instance;

	BuddyListModel *ChatsBuddiesModel;
	ChatsListModel *ChatsModel;
	TalkableTreeView *ChatsTalkableTree;
	ModelChain *ChatsModelChain;

	BuddyListModel *StatusBuddiesModel;
	TalkableTreeView *StatusesTalkableTree;
	ModelChain *StatusesModelChain;

	QStandardItemModel *SmsModel;
	QListView *SmsListView;

	QTreeView *DetailsListView;

	ChatDatesModel *MyChatDatesModel;
	BuddyStatusDatesModel *MyBuddyStatusDatesModel;
	SmsDatesModel *MySmsDatesModel;

	QMenu *DetailsPopupMenu;

	ChatMessagesView *ContentBrowser;

	explicit HistoryWindow(QWidget *parent = 0);
	virtual ~HistoryWindow();

	void createGui();
	void createTrees(QWidget *parent);
	QWidget * createChatTree(QWidget *parent);
	QWidget * createStatusTree(QWidget *parent);
	QWidget * createSMSTree(QWidget *parent);
	void connectGui();

	void updateData();
	void selectChat(const Chat &chat);

	void chatActivated(const Chat &chat);
	void statusBuddyActivated(const Buddy &buddy);
	void smsRecipientActivated(const QString &recipient);

	QVector<Message> statusesToMessages(const QList<TimedStatus> &statuses);

private slots:
	void currentChatChanged(const Talkable &talkable);
	void currentStatusChanged(const Talkable &talkable);
	void currentSmsChanged(const QModelIndex &current, const QModelIndex &previous);

	void dateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	void showChatsPopupMenu(const QPoint &pos);
	void showStatusesPopupMenu(const QPoint &pos);
	void showSmsPopupMenu(const QPoint &pos);

	void showDetailsPopupMenu(const QPoint &pos);

	void clearChatHistory();
	void clearStatusHistory();
	void clearSmsHistory();
	void removeHistoryEntriesPerDate();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static void show(const Chat &chat);

};

#endif // HISTORY_WINDOW_H
