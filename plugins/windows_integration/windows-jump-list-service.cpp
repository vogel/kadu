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

#include "chat/recent-chat-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QWidget>

#ifdef Q_OS_WIN
#	include <QtWinExtras/QtWinExtras>
#endif

WindowsJumpListService::WindowsJumpListService(QObject *parent) :
		QObject{parent}
{
#ifdef Q_OS_WIN
	m_jumpList = make_owned<QWinJumpList>(this);
#endif
}

WindowsJumpListService::~WindowsJumpListService()
{
}

void WindowsJumpListService::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
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

	updateJumpList();
}

void WindowsJumpListService::updateJumpList()
{
#ifdef Q_OS_WIN
	m_jumpList->tasks()->clear();

	for (auto const chat : m_recentChatManager->recentChats())
		addChat(chat);

	addSeparator();

	for (auto const chatWidget : m_chatWidgetRepository)
		addChat(chatWidget->chat());

	m_jumpList->tasks()->setVisible(m_jumpList->tasks()->count() > 0);
#endif
}

void WindowsJumpListService::addChat(Chat chat)
{
#ifdef Q_OS_WIN
	auto title = chat.display().isEmpty() ? chat.name() : chat.display();
	m_jumpList->tasks()->addLink(title, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()), QStringList{"--open-uuid", chat.uuid().toString()});
#endif
}

void WindowsJumpListService::addSeparator()
{
#ifdef Q_OS_WIN
	m_jumpList->tasks()->addSeparator();
#endif
}

#include "windows-jump-list-service.moc"
