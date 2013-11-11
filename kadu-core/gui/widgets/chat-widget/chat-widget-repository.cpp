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

#include "chat-widget-repository.h"

#include "gui/widgets/chat-widget/chat-widget.h"

ChatWidgetRepository::ChatWidgetRepository(QObject *parent) :
		QObject(parent)
{
}

ChatWidgetRepository::~ChatWidgetRepository()
{
}

ChatWidgetRepositoryIterator ChatWidgetRepository::begin()
{
	return ChatWidgetRepositoryIterator{m_widgets.begin()};
}

ChatWidgetRepositoryIterator ChatWidgetRepository::end()
{
	return ChatWidgetRepositoryIterator{m_widgets.end()};
}

void ChatWidgetRepository::addChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget || m_widgets.contains(chatWidget->chat()))
		return;

	connect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(widgetDestroyed(ChatWidget*)));
	m_widgets.insert(chatWidget->chat(), chatWidget);
	emit chatWidgetAdded(chatWidget);
}

void ChatWidgetRepository::removeChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget || !m_widgets.contains(chatWidget->chat()))
		return;

	disconnect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(widgetDestroyed(ChatWidget*)));
	m_widgets.remove(chatWidget->chat());
	emit chatWidgetRemoved(chatWidget);
}

ChatWidget * ChatWidgetRepository::widgetForChat(const Chat &chat)
{
	if (!chat)
		return nullptr;

	return m_widgets.value(chat);
}

const QMap<Chat, ChatWidget *> & ChatWidgetRepository::widgets() const
{
	return m_widgets;
}

void ChatWidgetRepository::widgetDestroyed(ChatWidget *chatWidget)
{
	removeChatWidget(chatWidget);
}

#include "moc_chat-widget-repository.cpp"
