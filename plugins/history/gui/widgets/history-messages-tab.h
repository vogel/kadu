/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HISTORY_MESSAGES_TAB_H
#define HISTORY_MESSAGES_TAB_H

#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>

#include "talkable/talkable.h"

#include "gui/widgets/history-tab.h"

class QDate;
class QMenu;
class QSplitter;

class ActionContext;
class BuddyListModel;
class ChatListModel;
class FilteredTreeView;
class HistoryQueryResult;
class HistoryMessagesStorage;
class Message;
class ModelChain;
class TalkableTreeView;
class TimelineChatMessagesView;
class WaitOverlay;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryMessagesTab
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that provides tab content for history window.
 *
 * This widgets provides tab content for history window - it can be chat, statuses, sms history
 * or any other item. This widgets adds menu with "Remove Entries" action for timeline widget.
 *
 * Each tab is build from one item view on the left and TimelineChatMessagesView on the right side.
 */
class HistoryMessagesTab : public HistoryTab
{
	Q_OBJECT

	HistoryMessagesStorage *Storage;

	QSplitter *Splitter;
	WaitOverlay *TabWaitOverlay;

	ChatListModel *ChatsModel;
	BuddyListModel *BuddiesModel;
	ModelChain *Chain;

	FilteredTreeView *FilteredView;
	TalkableTreeView *TalkableTree;
	QString ClearHistoryMenuItemTitle;
	QMenu *TimelinePopupMenu;
	TimelineChatMessagesView *TimelineView;

	QFutureWatcher<QVector<Talkable> > *TalkablesFutureWatcher;

	Talkable CurrentTalkable;

	void createGui();
	void createModelChain();

private slots:
	void futureTalkablesAvailable();
	void futureTalkablesCanceled();

	void currentTalkableChanged(const Talkable &talkable);
	void currentDateChanged();

	void showTalkablePopupMenu();
	void clearTalkableHistory();

	void showTimelinePopupMenu();
	void removeEntries();

protected:
	Talkable currentTalkable() const { return CurrentTalkable; }
	void displayTalkable(const Talkable &talkable, bool force);

	virtual void keyPressEvent(QKeyEvent *event);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of talkables to display in tree view.
	 * @param talkables talkables to display in tree view
	 *
	 * This methods sets list of talkable items to display in tree view.
	 */
	void setTalkables(const QVector<Talkable> &talkables);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets future list of talkables to display in tree view.
	 * @param futureTalkables future talkables to display in tree view
	 *
	 * This methods sets list of future talkable items to display in tree view. Whole tab will
	 * be blocked by WaitOverlay until talkables are available.
	 */
	void setFutureTalkables(const QFuture<QVector<Talkable>> &futureTalkables);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns FilteredTreeView widget used in this tab.
	 * @return FilteredTreeView widget used in this tab
	 */
	FilteredTreeView * filteredView() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns TalkableTreeView widget used in this tab.
	 * @return TalkableTreeView widget used in this tab
	 */
	TalkableTreeView * talkableTree() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns ModelChain widget used in this tab's tree view.
	 * @return ModelChain widget used in this tab's tree view
	 */
	ModelChain * modelChain() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Show wait overlay over tab.
	 */
	void showTabWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Hide wait overlay over tab.
	 */
	void hideTabWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after talkables are received from future objects.
	 */
	virtual void talkablesAvailable();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new HistoryTab widget.
	 * @param parent parent widget
	 *
	 * This contructor cannot be called directly, as this class is abstract.
	 */
	explicit HistoryMessagesTab(QWidget *parent = 0);
	virtual ~HistoryMessagesTab();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns timeline view widget.
	 * @return timeline view widget
	 */
	TimelineChatMessagesView * timelineView() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Allows to change title of "Delete history" menu item.
	 * @param clearHistoryMenuItemTitle new title of "Delete history" menu item.
	 */
	void setClearHistoryMenuItemTitle(const QString &clearHistoryMenuItemTitle);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates data in given chat.
	 *
	 * This method updates data in given chat. List of items in item list widget is updated with
	 * newsest data from history storage. List of dates in timeline widget is cleared and nothing
	 * is selected.
	 */
	virtual void updateData();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns sizes of subwidgets.
	 * @return sizes of subwidgets
	 *
	 * This list contains 4 values: width of list item widget, width of timeline view widget, height
	 * of timeline widget and height of chat messages view widget. It is enought to describe sized
	 * of whole widget.
	 */
	QList<int> sizes() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets sizes of subwidgets.
	 * @param newSizes new sizes of subwidgets
	 *
	 * Passed list must contain 4 values: new width of list item widget, new width of timeline view widget,
	 * new height of timeline widget and new height of chat messages view widget.
	 * If it does not, assertion is thrown.
	 */
	void setSizes(const QList<int> &newSizes);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets HistoryMessagesStorage to use by this tab.
	 * @param storage new storage to use
	 */
	void setHistoryMessagesStorage(HistoryMessagesStorage *storage);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns HistoryMessagesStorage used by this tab.
	 * @return HistoryMessagesStorage used by this tab
	 */
	HistoryMessagesStorage * historyMessagesStorage() const;

};

/**
 * @}
 */

#endif // HISTORY_MESSAGES_TAB_H
