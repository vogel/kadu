/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-container-handler-repository.h"

ChatWidgetContainerHandlerRepository::ChatWidgetContainerHandlerRepository(QObject *parent): QObject(parent)
{
}

ChatWidgetContainerHandlerRepository::~ChatWidgetContainerHandlerRepository()
{
}

void ChatWidgetContainerHandlerRepository::registerChatWidgetContainerHandler(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	m_chatWidgetContainerHandlers.prepend(chatWidgetContainerHandler);
	emit chatWidgetContainerHandlerRegistered(chatWidgetContainerHandler);
}

void ChatWidgetContainerHandlerRepository::unregisterChatWidgetContainerHandler(ChatWidgetContainerHandler *chatWidgetContainerHandler)
{
	m_chatWidgetContainerHandlers.removeAll(chatWidgetContainerHandler);
	emit chatWidgetContainerHandlerUnregistered(chatWidgetContainerHandler);
}

ChatWidgetContainerHandlerRepository::iterator ChatWidgetContainerHandlerRepository::begin()
{
	return m_chatWidgetContainerHandlers.begin();
}

ChatWidgetContainerHandlerRepository::iterator ChatWidgetContainerHandlerRepository::end()
{
	return m_chatWidgetContainerHandlers.end();
}

QList<ChatWidgetContainerHandler *> ChatWidgetContainerHandlerRepository::chatWidgetContainerHandlers() const
{
	return m_chatWidgetContainerHandlers;
}

#include "moc_chat-widget-container-handler-repository.cpp"
