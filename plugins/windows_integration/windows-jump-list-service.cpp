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

#include "chat/open-chat-repository.h"
#include "chat/recent-chat-repository.h"

WindowsJumpListService::WindowsJumpListService(QObject *parent) :
		QObject{parent}
{
}

WindowsJumpListService::~WindowsJumpListService()
{
}

void WindowsJumpListService::setJumpList(JumpList *jumpList)
{
	m_jumpList = jumpList;
}

void WindowsJumpListService::setOpenChatRepository(OpenChatRepository *openChatRepository)
{
	m_openChatRepository = openChatRepository;
}

void WindowsJumpListService::setRecentChatRepository(RecentChatRepository *recentChatRepository)
{
	m_recentChatRepository = recentChatRepository;
}

void WindowsJumpListService::init()
{
	connect(m_recentChatRepository, &RecentChatRepository::recentChatAdded, this, &WindowsJumpListService::updateJumpList);
	connect(m_recentChatRepository, &RecentChatRepository::recentChatRemoved, this, &WindowsJumpListService::updateJumpList);
	connect(m_openChatRepository, &OpenChatRepository::openChatAdded, this, &WindowsJumpListService::updateJumpList);
	connect(m_openChatRepository, &OpenChatRepository::openChatRemoved, this, &WindowsJumpListService::updateJumpList);

	updateJumpList();
}

void WindowsJumpListService::updateJumpList()
{
	auto needSeparator = false;
	auto any = false;

	m_jumpList->clear();
	for (auto const &chat : m_recentChatRepository)
		if (std::find(begin(m_openChatRepository), end(m_openChatRepository), chat) == end(m_openChatRepository))
		{
			needSeparator = true;
			m_jumpList->addChat(chat);
			any = true;
		}

	for (auto const &chat : m_openChatRepository)
	{
		if (needSeparator)
		{
			m_jumpList->addSeparator();
			needSeparator = false;
		}
		m_jumpList->addChat(chat);
		any = true;
	}

	m_jumpList->setVisible(any);
}

#include "windows-jump-list-service.moc"
