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

#include "chat-window-repository.h"

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/chat-window/chat-window.h"

ChatWindowRepository::ChatWindowRepository(QObject *parent) :
		QObject{parent}
{
}

ChatWindowRepository::~ChatWindowRepository()
{
}

void ChatWindowRepository::addChatWindow(ChatWindow *chatWindow)
{
	if (!chatWindow || m_windows.contains(chatWindow->chatWidget()))
		return;

	connect(chatWindow, SIGNAL(windowDestroyed(ChatWindow*)), this, SLOT(windowDestroyed(ChatWindow*)));
	m_windows.insert(chatWindow->chatWidget(), chatWindow);
}

void ChatWindowRepository::removeChatWindow(ChatWindow *chatWindow)
{
	if (!chatWindow || !m_windows.contains(chatWindow->chatWidget()))
		return;

	disconnect(chatWindow, SIGNAL(windowDestroyed(ChatWindow*)), this, SLOT(windowDestroyed(ChatWindow*)));
	m_windows.remove(chatWindow->chatWidget());
}

ChatWindow * ChatWindowRepository::windowForChatWidget(ChatWidget * const chatWidget)
{
	if (!chatWidget)
		return nullptr;

	return m_windows.value(chatWidget);
}

const QMap<ChatWidget *, ChatWindow *> &ChatWindowRepository::windows() const
{
	return m_windows;
}

void ChatWindowRepository::windowDestroyed(ChatWindow *chatWindow)
{
	removeChatWindow(chatWindow);
}

#include "moc_chat-window-repository.cpp"
