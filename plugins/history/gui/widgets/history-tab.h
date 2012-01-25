/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_TAB_H
#define HISTORY_TAB_H

#include <QtCore/QFuture>
#include <QtCore/QFutureWatcher>
#include <QtGui/QWidget>

#include "exports.h"

class QDate;
class QMenu;
class QSplitter;

class BuddyListModel;
class ChatsListModel;
class DatesModelItem;
class FilteredTreeView;
class HistoryDatesModel;
class HistoryMessagesStorage;
class Message;
class ModelChain;
class Talkable;
class TalkableTreeView;
class TimelineChatMessagesView;
class WaitOverlay;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryTab
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that provides tab content for history window.
 *
 * This widgets provides tab content for history window - it can be chat, statuses, sms history
 * or any other item. This widgets adds menu with "Remove Entries" action for timeline widget.
 *
 * Each tab is build from one item view on the left and TimelineChatMessagesView on the right side.
 * Every implementation of HistoryTab must set ModelChain on talkableTree(), implement displayForDate()
 * to update view for selected date, removeEntriesPerDate() to remove history entries for given date
 * and currentTalkableChanged() to update list of available dates.
 */
class KADUAPI HistoryTab : public QWidget
{
	Q_OBJECT

	HistoryMessagesStorage *Storage;

	QSplitter *Splitter;
	WaitOverlay *TabWaitOverlay;
	WaitOverlay *TimelineWaitOverlay;
	WaitOverlay *MessagesViewWaitOverlay;

	ChatsListModel *ChatsModel;
	BuddyListModel *BuddiesModel;
	ModelChain *Chain;

	FilteredTreeView *FilteredView;
	TalkableTreeView *TalkableTree;
	QMenu *TimelinePopupMenu;
	TimelineChatMessagesView *TimelineView;
	HistoryDatesModel *DatesModel;

	QFutureWatcher<QVector<Talkable> > *TalkablesFutureWatcher;
	QFutureWatcher<QVector<DatesModelItem> > *DatesFutureWatcher;
	QFutureWatcher<QVector<Message> > *MessagesFutureWatcher;

	void createGui();
	void createModelChain();

private slots:
	void futureTalkablesAvailable();
	void futureTalkablesCanceled();

	void futureDatesAvailable();
	void futureDatesCanceled();

	void futureMessagesAvailable();
	void futureMessagesCanceled();

	void currentDateChanged();

	void showTalkablePopupMenu();
	void showTimelinePopupMenu();
	void removeEntries();

protected:
	void keyPressEvent(QKeyEvent *event);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns timeline view widget.
	 * @return timeline view widget
	 */
	TimelineChatMessagesView * timelineView() const;

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
	void setFutureTalkables(const QFuture<QVector<Talkable> > &futureTalkables);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of dates to display in timeline.
	 * @param dates dates to display in timeline
	 *
	 * This methods sets list of dates to display in timeline. If list is not empty,
	 * last date is selected and displayForDate() is called with that date. IF not,
	 * displayForDate() is called with invalid date to ensure that view is cleared.
	 */
	void setDates(const QVector<DatesModelItem> &dates);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets future list of dates to display in timeline.
	 * @param futureDates future dates to display in timeline
	 *
	 * This methods sets list of future dates to display in timeline. Timeline view will
	 * be blocked by WaitOverlay until dates are available. If received list will be not empty
	 * last date will be selected and displayForDate() will be called with that date.
	 * If received list will be empty, displayForDate() will be called with invalid date to ensure
	 * that view is cleared.
	 */
	void setFutureDates(const QFuture<QVector<DatesModelItem> > &futureDates);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets messages to display in message view widget.
	 * @param messages future messages of dates to display in message view widget
	 *
	 * This methods sets list of messages to display in message view widget.
	 */
	void setMessages(const QVector<Message> &messages);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets future messages to display in message view widget.
	 * @param futureMessages future messages of dates to display in message view widget
	 *
	 * This methods sets list of future messages to display in message view widget. This widget will
	 * be blocked by WaitOverlay until messages are available.
	 */
	void setFutureMessages(const QFuture<QVector<Message> > &futureMessages);

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
	 * @short Show wait overlay over timeline widget.
	 */
	void showTimelineWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Hide wait overlay over timeline widget.
	 */
	void hideTimelineWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Show wait overlay over messages view widget.
	 */
	void showMessagesViewWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Hide wait overlay over messages view widget.
	 */
	void hideMessagesViewWaitOverlay();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Allows to change content of default popup menu displayed on items.
	 * @param menu menu to change
	 */
	virtual void modifyTalkablePopupMenu(const QScopedPointer<QMenu> &menu) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called after talkables are received from future objects.
	 */
	virtual void talkablesAvailable();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called to update chat messages view with date from given date.
	 * @param date selected date
	 *
	 * This method is called every time user changes selection of date in timeline widget (or
	 * when list of dates is changed). If date is valid, messages view must be filled with data
	 * for this date. If date is not valid, messages view must be cleared.
	 */
	virtual void displayForDate(const QDate &date) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called to remove entries for given date.
	 * @param date selected date
	 *
	 * This method is called when user selects "Remove Entries" action from timeline popup menu.
	 * If date is valid, entries for given date must be removed from history storage.
	 * After removal updateData() method should be called to ensure consistency.
	 */
	virtual void removeEntriesPerDate(const QDate &date) = 0;

protected slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when current talkable in item view changes.
	 * @param talkable current talkable
	 *
	 * In this method list of available dates should be updated by calling setDates() or setFutureDates().
	 */
	virtual void currentTalkableChanged(const Talkable &talkable) = 0;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new HistoryTab widget.
	 * @param showTitleInTimeline if true, timeline view has 3 columns, not 2
	 * @param parent parent widget
	 *
	 * This contructor cannot be called directly, as this class is abstract. Implementations must
	 * call this method and choose whether to use 3 or 2 columns in timeline view.
	 */
	explicit HistoryTab(bool showTitleInTimeline, QWidget *parent = 0);
	virtual ~HistoryTab();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Updates data in given chat.
	 *
	 * This method updates data in given chat. List of items in item list widget is updated with
	 * newsest data from history storage. List of dates in timeline widget is cleared and nothing
	 * is selected.
	 */
	virtual void updateData() = 0;

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

#endif // HISTORY_TAB_H
