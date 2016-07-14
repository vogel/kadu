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

#include "windows-jump-list-service.h"

#include "jump-list.h"

#include "chat/recent-chat-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

WindowsJumpListService::WindowsJumpListService(QObject *parent) :
		QObject{parent}
{
}

WindowsJumpListService::~WindowsJumpListService()
{
}

void WindowsJumpListService::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void WindowsJumpListService::setJumpList(JumpList *jumpList)
{
	m_jumpList = jumpList;
}

void WindowsJumpListService::setRecentChatManager(RecentChatManager *recentChatManager)
{
	m_recentChatManager = recentChatManager;
}

void WindowsJumpListService::init()
{
	connect(m_recentChatManager, &RecentChatManager::recentChatAdded, this, &WindowsJumpListService::updateJumpList);
	connect(m_recentChatManager, &RecentChatManager::recentChatRemoved, this, &WindowsJumpListService::updateJumpList);
	connect(m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetAdded, this, &WindowsJumpListService::updateJumpList);
	connect(m_chatWidgetRepository, &ChatWidgetRepository::chatWidgetRemoved, this, &WindowsJumpListService::updateJumpList);

	m_recentChatManager->recentChats();

	updateJumpList();
}

void WindowsJumpListService::updateJumpList()
{
	auto needSeparator = false;

	m_jumpList->clear();
	for (auto const chat : m_recentChatManager->recentChats())
	{
		needSeparator = true;
		m_jumpList->addChat(chat);
	}

	for (auto const chatWidget : m_chatWidgetRepository)
	{
		if (needSeparator)
		{
			m_jumpList->addSeparator();
			needSeparator = false;
		}
		m_jumpList->addChat(chatWidget->chat());
	}
}

#include "windows-jump-list-service.moc"
