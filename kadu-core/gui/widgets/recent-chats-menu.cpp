/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/model/chat-data-extractor.h"
#include "chat/recent-chat-repository.h"
#include "chat/type/chat-type-manager.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"

#include "recent-chats-menu.h"


RecentChatsMenu::RecentChatsMenu(QWidget *parent) :
		QMenu(parent)
{
}

RecentChatsMenu::~RecentChatsMenu()
{
}

void RecentChatsMenu::setChatDataExtractor(ChatDataExtractor *chatDataExtractor)
{
	m_chatDataExtractor = chatDataExtractor;
}

void RecentChatsMenu::setChatTypeManager(ChatTypeManager *chatTypeManager)
{
	m_chatTypeManager = chatTypeManager;
}

void RecentChatsMenu::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	m_chatWidgetRepository = chatWidgetRepository;
}

void RecentChatsMenu::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void RecentChatsMenu::setRecentChatRepository(RecentChatRepository *recentChatRepository)
{
	m_recentChatRepository = recentChatRepository;
}

void RecentChatsMenu::init()
{
	setIcon(m_iconsManager->iconByPath(KaduIcon("internet-group-chat")));
	setTitle(tr("Recent chats"));

	connect(m_iconsManager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetAdded(ChatWidget*)), this, SLOT(invalidate()));
	connect(m_chatWidgetRepository, SIGNAL(chatWidgetRemoved(ChatWidget*)), this, SLOT(invalidate()));
	connect(m_recentChatRepository, SIGNAL(recentChatAdded(Chat)), this, SLOT(invalidate()));
	connect(m_recentChatRepository, SIGNAL(recentChatRemoved(Chat)), this, SLOT(invalidate()));
	connect(this, SIGNAL(aboutToShow()), this, SLOT(update()));

	invalidate();
}

void RecentChatsMenu::invalidate()
{
	m_recentChatsMenuNeedsUpdate = true;

	checkIfListAvailable();
}

void RecentChatsMenu::checkIfListAvailable()
{
	//check if all recent chats are opened -> disable button
	for (auto const &chat : m_recentChatRepository)
		if (!m_chatWidgetRepository->widgetForChat(chat))
		{
			emit chatsListAvailable(true);
			return;
		}

	emit chatsListAvailable(false);
}

void RecentChatsMenu::update()
{
	if (!m_recentChatsMenuNeedsUpdate)
		return;

	clear();

	for (auto const &chat : m_recentChatRepository)
		if (!m_chatWidgetRepository->widgetForChat(chat))
		{
			ChatType *type = m_chatTypeManager->chatType(chat.type());
			QAction *action = new QAction(type ? m_iconsManager->iconByPath(type->icon()) : QIcon(),
			                              m_chatDataExtractor->data(chat, Qt::DisplayRole).toString(),
			                              this);
			action->setData(QVariant::fromValue<Chat>(chat));
			this->addAction(action);
		}

	emit chatsListAvailable(!actions().isEmpty());

	m_recentChatsMenuNeedsUpdate = false;
}

void RecentChatsMenu::iconThemeChanged()
{
	setIcon(m_iconsManager->iconByPath(KaduIcon("internet-group-chat")));
}

#include "moc_recent-chats-menu.cpp"
