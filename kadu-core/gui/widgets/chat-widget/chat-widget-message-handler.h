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
#include <QtCore/QWeakPointer>

class ChatWidget;
class ChatWidgetRepository;
class UnreadMessageRepository;

class ChatWidgetMessageHandler : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetMessageHandler(QObject *parent = nullptr);
	virtual ~ChatWidgetMessageHandler();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);

private:
	QWeakPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QWeakPointer<UnreadMessageRepository> m_unreadMessageRepository;

private slots:
	void chatWidgetCreated(ChatWidget *chatWidget);
	void chatWidgetDestroyed(ChatWidget *chatWidget);
	void chatWidgetActivated(ChatWidget *chatWidget);

};
