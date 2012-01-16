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
class ChatsListModel;
class HistoryDatesModel;
class ModelChain;
class NameTalkableFilter;
class SmsDatesModel;
class TalkableTreeView;
class TimedStatus;
class TimelineChatMessagesView;

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

	TimelineChatMessagesView *TimelineChatView;
	TimelineChatMessagesView *TimelineStatusesView;
	TimelineChatMessagesView *TimelineSmsesView;

	HistoryDatesModel *MyChatDatesModel;
	BuddyStatusDatesModel *MyBuddyStatusDatesModel;
	SmsDatesModel *MySmsDatesModel;

	QMenu *ChatDetailsPopupMenu;
	QMenu *StatusDetailsPopupMenu;
	QMenu *SmsDetailsPopupMenu;

	explicit HistoryWindow(QWidget *parent = 0);
	virtual ~HistoryWindow();

	void createGui();
	QWidget * createChatTab(QWidget *parent);
	QWidget * createStatusTab(QWidget *parent);
	QWidget * createSmsTab(QWidget *parent);
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

	void chatDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void statusDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
	void smsDateCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	void showChatsPopupMenu(const QPoint &pos);
	void showStatusesPopupMenu(const QPoint &pos);
	void showSmsPopupMenu(const QPoint &pos);

	void showChatDetailsPopupMenu(const QPoint &pos);
	void showStatusDetailsPopupMenu(const QPoint &pos);
	void showSmsDetailsPopupMenu(const QPoint &pos);

	void clearChatHistory();
	void clearStatusHistory();
	void clearSmsHistory();

	void removeChatEntriesPerDate();
	void removeStatusEntriesPerDate();
	void removeSmsEntriesPerDate();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static void show(const Chat &chat);

};

#endif // HISTORY_WINDOW_H
