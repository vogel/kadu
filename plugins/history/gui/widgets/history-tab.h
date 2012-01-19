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

#include <QtGui/QWidget>

#include "exports.h"

class QDate;
class QMenu;
class QSplitter;

class DatesModelItem;
class HistoryDatesModel;
class TimelineChatMessagesView;

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
 * Every implementation of HistoryTab must implement createTreeView() to create item view,
 * displayForDate() to update view for selected date and removeEntriesPerDate() to remove history entries
 * for given date.
 */
class KADUAPI HistoryTab : public QWidget
{
	Q_OBJECT

	QSplitter *Splitter;

	QMenu *TimelinePopupMenu;
	TimelineChatMessagesView *TimelineView;
	HistoryDatesModel *DatesModel;

private slots:
	void currentDateChanged();

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
	 * @short Create gui for this tab.
	 *
	 * This methods must be called in implementations constructor, as it call abstract method
	 * createTreeView() to fill left part of the widget.
	 */
	void createGui();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create item view for this tab.
	 *
	 * This method must create a new item view that will be put on left side of this window.
	 * Implementations must ensure that changing selection in this view will update dates
	 * using setDates() method to reflect dates available for given selection.
	 */
	virtual void createTreeView(QWidget *parent) = 0;

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

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new HistoryTab widget.
	 * @param showTitleInTimeline if true, timeline view has 3 columns, not 2
	 * @param parent parent widget
	 *
	 * This contructor cannot be called directly, as this class is abstract. Implementations must
	 * call this method and choose whether to use 3 or 2 columns in timeline view.
	 *
	 * Implementations must call createGui() in their own constructors, as createGui() calls abstract
	 * method createTreeView(), so it cannot be called by HistoryTab() construtor.
	 *
	 * This is not perfect, but it is good enough for Kadu 0.12.0 and later editions.
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

};

/**
 * @}
 */

#endif // HISTORY_TAB_H
