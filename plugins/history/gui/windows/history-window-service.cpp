/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-window-service.h"

#include "gui/windows/history-window.h"
#include "history.h"

#include "chat/buddy-chat-manager.h"
#include "core/injected-factory.h"
#include "activate.h"

HistoryWindowService::HistoryWindowService(QObject *parent) :
		QObject{parent}
{
}

HistoryWindowService::~HistoryWindowService()
{
}

void HistoryWindowService::setBuddyChatManager(BuddyChatManager *buddyChatManager)
{
	m_buddyChatManager = buddyChatManager;
}

void HistoryWindowService::setHistory(History *history)
{
	m_history = history;
}

void HistoryWindowService::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void HistoryWindowService::show(const Chat &chat)
{
	auto buddyChat = m_buddyChatManager->buddyChat(chat);
	if (!buddyChat)
		buddyChat = chat;

	if (!m_historyWindow)
		m_historyWindow = m_injectedFactory->makeInjected<HistoryWindow>();

	m_historyWindow->updateData();
	m_historyWindow->selectChat(buddyChat);

	m_historyWindow->setVisible(true);
	_activateWindow(m_historyWindow);
}
