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

class KADUAPI TimelineChatMessagesView : public QWidget
{
	Q_OBJECT

	QSplitter *Splitter;
	QTreeView *Timeline;
	ChatMessagesView *MessagesView;

	void createGui();

public:
	explicit TimelineChatMessagesView(QWidget *parent = 0);
	virtual ~TimelineChatMessagesView();

	QTreeView * timeline() const { return Timeline; }
	ChatMessagesView * messagesView() const { return MessagesView; }

	QDate currentDate() const;

	QList<int> sizes() const;
	void setSizes(const QList<int> &newSizes);

};

#endif // TIMELINE_CHAT_MESSAGES_VIEW_H
