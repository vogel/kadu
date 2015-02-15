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

#include "chat-window-repository.h"

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/chat-window/chat-window.h"

ChatWindow * ChatWindowRepository::converter(ChatWindowRepository::WrappedIterator iterator)
{
	return iterator->second;
}

ChatWindowRepository::ChatWindowRepository(QObject *parent) :
		QObject{parent}
{
}

ChatWindowRepository::~ChatWindowRepository()
{
	// neeed to emit signals on finish
	while (!m_windows.empty())
		delete (*m_windows.begin()).second;
}

ChatWindowRepository::Iterator ChatWindowRepository::begin()
{
	return Iterator{m_windows.begin(), converter};
}

ChatWindowRepository::Iterator ChatWindowRepository::end()
{
	return Iterator{m_windows.end(), converter};
}

void ChatWindowRepository::addChatWindow(ChatWindow *chatWindow)
{
	if (!chatWindow || hasWindowForChat(chatWindow->chat()))
		return;

	m_windows.insert(std::make_pair(chatWindow->chat(), chatWindow));
	connect(chatWindow, SIGNAL(windowDestroyed(ChatWindow*)), this, SLOT(removeChatWindow(ChatWindow*)));
}

void ChatWindowRepository::removeChatWindow(ChatWindow *chatWindow)
{
	if (!chatWindow || !hasWindowForChat(chatWindow->chat()))
		return;

	m_windows.erase(chatWindow->chat());
}

bool ChatWindowRepository::hasWindowForChat(const Chat &chat) const
{
	return m_windows.end() != m_windows.find(chat);
}

ChatWindow * ChatWindowRepository::windowForChat(const Chat &chat)
{
	if (!chat)
		return nullptr;

	auto it = m_windows.find(chat);
	return it != m_windows.end()
			? it->second
			: nullptr;
}

#include "moc_chat-window-repository.cpp"
