/*
 * %kadu copyright begin%
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-repository-impl.h"

#include "gui/widgets/chat-widget/chat-widget.h"

ChatWidget * ChatWidgetRepositoryImpl::converter(ChatWidgetRepositoryImpl::WrappedIterator iterator)
{
	return iterator->second;
}

ChatWidgetRepositoryImpl::ChatWidgetRepositoryImpl(QObject *parent) :
		ChatWidgetRepository{parent}
{
}

ChatWidgetRepositoryImpl::~ChatWidgetRepositoryImpl()
{
}

ChatWidgetRepositoryImpl::Iterator ChatWidgetRepositoryImpl::begin()
{
	return Iterator{m_widgets.begin(), converter};
}

ChatWidgetRepositoryImpl::Iterator ChatWidgetRepositoryImpl::end()
{
	return Iterator{m_widgets.end(), converter};
}

void ChatWidgetRepositoryImpl::addChatWidget(ChatWidget *chatWidget)
{
	if (!chatWidget || hasWidgetForChat(chatWidget->chat()))
		return;

	m_widgets.insert(std::make_pair(chatWidget->chat(), chatWidget));
	emit chatWidgetAdded(chatWidget);

	connect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(removeChatWidget(ChatWidget*)));
}

void ChatWidgetRepositoryImpl::removeChatWidget(ChatWidget *chatWidget)
{
	auto chat = chatWidget->chat();
	if (!chatWidget || (widgetForChat(chat) != chatWidget))
		return;

	m_widgets.erase(chat);
	emit chatWidgetRemoved(chatWidget);
}

void ChatWidgetRepositoryImpl::removeChatWidget(Chat chat)
{
	removeChatWidget(widgetForChat(chat));
}

bool ChatWidgetRepositoryImpl::hasWidgetForChat(const Chat &chat) const
{
	return m_widgets.end() != m_widgets.find(chat);
}

ChatWidget * ChatWidgetRepositoryImpl::widgetForChat(const Chat &chat)
{
	if (!chat)
		return nullptr;

	auto it = m_widgets.find(chat);
	return it != m_widgets.end()
			? it->second
			: nullptr;
}

#include "moc_chat-widget-repository-impl.cpp"
