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

#include <QtGui/QWidget>

#include "exports.h"

class QDate;
class QSplitter;
class QTreeView;

class ChatMessagesView;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class TimelineChatMessagesView
 * @author Rafał 'Vogel' Malinowski
 * @short Widget that provides chat message view with timeline.
 *
 * This widgets mixes list of dates with chat message view in vertical layout.
 * It is used in history window.
 */
class KADUAPI TimelineChatMessagesView : public QWidget
{
	Q_OBJECT

	QSplitter *Splitter;
	QTreeView *Timeline;
	ChatMessagesView *MessagesView;

	void createGui();

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
	 * @short Returns currently selected date on timeline.
	 * @return currently selected date on timeline
	 *
	 * If no data is seleted then returned value is invalid.
	 */
	QDate currentDate() const;

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

};

/**
 * @}
 */

#endif // TIMELINE_CHAT_MESSAGES_VIEW_H
