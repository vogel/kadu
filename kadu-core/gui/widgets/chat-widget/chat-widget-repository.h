/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>

#include "chat/chat.h"
#include "exports.h"

class ChatWidget;
class ChatWidgetFactory;

class KADUAPI ChatWidgetRepository : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetRepository(QObject *parent = 0);
	virtual ~ChatWidgetRepository();

	void setChatWidgetFactory(ChatWidgetFactory *chatWidgetFactory);

	bool hasWidgetForChat(const Chat &chat) const;
	ChatWidget * widgetForChat(const Chat &chat);
	const QMap<Chat, ChatWidget *> & widgets() const;

signals:
	void chatWidgetCreated(ChatWidget *chatWidget);
	void chatWidgetDestroyed(ChatWidget *chatWidget);

private:
	QWeakPointer<ChatWidgetFactory> m_chatWidgetFactory;
	QMap<Chat, ChatWidget *> m_widgets;

private slots:
	void widgetDestroyed(const Chat &chat);

};
