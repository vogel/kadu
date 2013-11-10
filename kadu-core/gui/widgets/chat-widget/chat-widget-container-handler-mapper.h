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

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class ChatWidget;
class ChatWidgetContainerHandler;
class ChatWidgetContainerHandlerRepository;
class ChatWidgetRepository;

class ChatWidgetContainerHandlerMapper : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetContainerHandlerMapper(QObject *parent = 0);
	virtual ~ChatWidgetContainerHandlerMapper();

	void setChatWidgetContainerHandlerRepository(ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository);
	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	bool isChatWidgetActive(ChatWidget *chatWidget) const;
	void tryActivateChatWidget(ChatWidget *chatWidget);

private:
	QWeakPointer<ChatWidgetContainerHandlerRepository> m_chatWidgetContainerHandlerRepository;
	QWeakPointer<ChatWidgetRepository> m_chatWidgetRepository;

	QMap<ChatWidget *, ChatWidgetContainerHandler *> m_mapping;

	void mapToDefault(ChatWidget *chatWidget);
	void map(ChatWidgetContainerHandler *chatWidgetContainerHandler, ChatWidget *chatWidget);
	void unmap(ChatWidget *chatWidget);
	ChatWidgetContainerHandler * bestContainerHandler(ChatWidget *chatWidget) const;

private slots:
	void chatWidgetContainerHandlerRegistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetContainerHandlerUnregistered(ChatWidgetContainerHandler *chatWidgetContainerHandler);
	void chatWidgetAcceptanceChanged(ChatWidgetContainerHandler *chatWidgetContainerHandler, ChatWidget *chatWidget);

	void chatWidgetAdded(ChatWidget *chatWidget);
	void chatWidgetRemoved(ChatWidget *chatWidget);

};
