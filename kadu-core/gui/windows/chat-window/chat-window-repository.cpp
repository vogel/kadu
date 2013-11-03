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

#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/chat-window/chat-window.h"
#include "gui/windows/chat-window/chat-window-factory.h"

#include "chat-window-repository.h"

ChatWindowRepository::ChatWindowRepository(QObject *parent) :
		QObject(parent)
{
}

ChatWindowRepository::~ChatWindowRepository()
{
}

void ChatWindowRepository::setChatWindowFactory(ChatWindowFactory *chatWindowFactory)
{
	m_chatWindowFactory = chatWindowFactory;
}

bool ChatWindowRepository::hasWindowForChatWidget(ChatWidget * const chatWidget) const
{
	return m_windows.contains(chatWidget);
}

ChatWindow * ChatWindowRepository::windowForChatWidget(ChatWidget * const chatWidget)
{
	if (!chatWidget)
		return nullptr;

	if (hasWindowForChatWidget(chatWidget))
		return m_windows.value(chatWidget);

	if (!m_chatWindowFactory)
		return nullptr;

	auto result = m_chatWindowFactory.data()->createChatWindow(chatWidget);
	connect(result.get(), SIGNAL(windowDestroyed(ChatWidget*const)), this, SLOT(windowDestroyed(ChatWidget*const)));
	m_windows.insert(chatWidget, result.get());

	return result.release();
}

const QMap<ChatWidget *, ChatWindow *> &ChatWindowRepository::windows() const
{
	return m_windows;
}

void ChatWindowRepository::windowDestroyed(ChatWidget * const chatWidget)
{
	if (!hasWindowForChatWidget(chatWidget))
		return;

	m_windows.remove(chatWidget);
}
