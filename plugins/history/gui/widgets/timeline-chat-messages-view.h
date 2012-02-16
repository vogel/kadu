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

#ifndef TIMELINE_CHAT_MESSAGES_VIEW_H
#define TIMELINE_CHAT_MESSAGES_VIEW_H

#include <QtCore/QFutureWatcher>
#include <QtGui/QWidget>

class QDate;
class QSplitter;
class QTreeView;

class ChatMessagesView;
class HistoryQueryResult;
class HistoryQueryResultsModel;
class HistoryQueryResultsProxyModel;
class Message;
class SearchBar;
class WaitOverlay;
class WebViewHighlighter;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class TimelineChatMessagesView
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that provides chat message view with timeline.
 *
 * This widgets mixes list of dates with chat message view in vertical layout. It is used in history window.
 * Search bar is attached to message view and can be invoked by Ctrl+F shortcut.
 */
class TimelineChatMessagesView : public QWidget
{
	Q_OBJECT

	WaitOverlay *TimelineWaitOverlay;
	WaitOverlay *MessagesViewWaitOverlay;

	QSplitter *Splitter;
	QTreeView *Timeline;
	HistoryQueryResultsModel *ResultsModel;
	HistoryQueryResultsProxyModel *ResultsProxyModel;
	WebViewHighlighter *Highlighter;
	ChatMessagesView *MessagesView;
	SearchBar *MessagesSearchBar;

	QFutureWatcher<QVector<HistoryQueryResult> > *ResultsFutureWatcher;
	QFutureWatcher<QVector<Message> > *MessagesFutureWatcher;

	void createGui();

private slots:
	void futureResultsAvailable();
	void futureResultsCanceled();

	void futureMessagesAvailable();
	void futureMessagesCanceled();

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Creates new TimelineChatMessagesView.
	 * @param parent parent widget
	 */
	explicit TimelineChatMessagesView(QWidget *parent = 0);
	virtual ~TimelineChatMessagesView();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns timeline widget.
	 * @return timeline widget
	 *
	 * Timeline widget is simple QTreeView with grid display of dates with messages count and
	 * optional title for each item.
	 */
	QTreeView * timeline() const { return Timeline; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns chat messages view widget.
	 * @return chat messages view widget
	 */
	ChatMessagesView * messagesView() const { return MessagesView; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns highlighter for chat messages view widget.
	 * @return highlighter for chat messages view widget
	 */
	WebViewHighlighter * highlighter() const { return Highlighter; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns chat messages view search bar widget.
	 * @return chat messages view search bar widget
	 */
	SearchBar * searchBar() const { return MessagesSearchBar; }

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns currently selected date on timeline.
	 * @return currently selected date on timeline
	 *
	 * If no data is seleted then returned value is invalid.
	 */
	QDate currentDate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets list of dates to display in timeline.
	 * @param dates dates to display in timeline
	 *
	 * This methods sets list of dates to display in timeline. If list is not empty,
	 * last date is selected and displayForDate() is called with that date. IF not,
	 * displayForDate() is called with invalid date to ensure that view is cleared.
	 */
	void setResults(const QVector<HistoryQueryResult> &results);

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
	void setFutureResults(const QFuture<QVector<HistoryQueryResult> > &futureResults);

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
	 * @short Decide if talkable name should be displayed in timeline widget.
	 * @param talkableVisible set to true if talkable name should be displayed in timeline widget
	 */
	void setTalkableVisible(const bool talkableVisible);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Decide if title should be displayed in timeline widget.
	 * @param titleVisible set to true if title should be displayed in timeline widget
	 */
	void setTitleVisible(const bool titleVisible);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set header to talkable column.
	 * @param talkableHeader header for talkable column
	 */
	void setTalkableHeader(const QString &talkableHeader);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set header to length column.
	 * @param lengthHeader header for length column
	 */
	void setLengthHeader(const QString &lengthHeader);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns heights of timeline and message view widgets.
	 * @return heights of timeline and message view widgets
	 *
	 * First value in list contains height of timeline widget. Second one contains height
	 * of chat message view widget. This method always returns list of size 2.
	 */
	QList<int> sizes() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets heights of timeline and message view widgets.
	 * @param newSizes new heights of timeline and message view widgets
	 *
	 * First value in list contains new height of timeline widget. Second one contains new height
	 * of chat message view widget. If size of parameter is different that 2 assertion is thrown.
	 */
	void setSizes(const QList<int> &newSizes);

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when selected date changes.
	 */
	void currentDateChanged();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emited when all messages are displayed.
	 */
	void messagesDisplayed();

};

/**
 * @}
 */

#endif // TIMELINE_CHAT_MESSAGES_VIEW_H
