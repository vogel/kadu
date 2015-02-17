/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-set-title.h"

#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/chat-widget/chat-widget-title.h"

ChatWidgetSetTitle::ChatWidgetSetTitle(QObject *parent) :
		QObject{parent}
{
}

ChatWidgetSetTitle::~ChatWidgetSetTitle()
{
}

void ChatWidgetSetTitle::setDefaultTile(QString defaultTitle)
{
	m_defaultTitle = std::move(defaultTitle);
	update();
}

void ChatWidgetSetTitle::setDefaultIcon(QIcon icon)
{
	m_defaultIcon = icon;
	update();
}

void ChatWidgetSetTitle::setActiveChatWidget(ChatWidget *chatWidget)
{
	m_activeChatWidget = chatWidget;
	if (!m_unreadMessagesChatWidget)
		setCurrentChatWidget(m_activeChatWidget);
}

QString ChatWidgetSetTitle::fullTitle() const
{
	if (m_unreadMessagesChatWidget)
		return m_unreadMessagesChatWidget->title()->blinkingFullTitle();
	if (m_currentChatWidget)
		return m_currentChatWidget->title()->blinkingFullTitle();
	return m_defaultTitle;
}

QIcon ChatWidgetSetTitle::icon() const
{
	if (m_unreadMessagesChatWidget)
		return m_unreadMessagesChatWidget->title()->icon();
	if (m_currentChatWidget)
		return m_currentChatWidget->title()->icon();
	return m_defaultIcon;
}

void ChatWidgetSetTitle::addChatWidget(ChatWidget *chatWidget)
{
	m_chatWidgets.append(chatWidget);
	connect(chatWidget->chat(), SIGNAL(updated()), this, SLOT(chatUpdated()));
	connect(chatWidget, SIGNAL(widgetDestroyed(Chat)), this, SLOT(chatWidgetDestroyed(Chat)));
	connect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(chatWidgetDestroyed(ChatWidget*)));

	if (!m_unreadMessagesChatWidget)
		selectNewUnreadMessagesChatWidget();
}

void ChatWidgetSetTitle::removeChatWidget(ChatWidget *chatWidget)
{
	disconnect(chatWidget, SIGNAL(widgetDestroyed(ChatWidget*)), this, SLOT(chatWidgetDestroyed(ChatWidget*)));
	disconnect(chatWidget->chat(), SIGNAL(updated()), this, SLOT(chatUpdated()));
	m_chatWidgets.removeAll(chatWidget);

	if (m_unreadMessagesChatWidget == chatWidget)
		selectNewUnreadMessagesChatWidget();
}

void ChatWidgetSetTitle::chatWidgetDestroyed(Chat chat)
{
	disconnect(chat, SIGNAL(updated()), this, SLOT(chatUpdated()));
}

void ChatWidgetSetTitle::chatWidgetDestroyed(ChatWidget *chatWidget)
{
	m_chatWidgets.removeAll(chatWidget);
	if (m_unreadMessagesChatWidget == m_currentChatWidget)
		selectNewUnreadMessagesChatWidget();
}

void ChatWidgetSetTitle::chatUpdated()
{
	auto chat = Chat{sender()};
	if (!chat)
		return;

	if (chat.unreadMessagesCount() == 0 && m_unreadMessagesChatWidget && m_unreadMessagesChatWidget->chat() == chat)
		selectNewUnreadMessagesChatWidget();
	if (chat.unreadMessagesCount() > 0 && !m_unreadMessagesChatWidget)
		selectNewUnreadMessagesChatWidget();
}

void ChatWidgetSetTitle::selectNewUnreadMessagesChatWidget()
{
	auto withUnreadMessages = std::find_if(std::begin(m_chatWidgets), std::end(m_chatWidgets), [](ChatWidget *chatWidget){
		return chatWidget->chat().unreadMessagesCount() > 0;
	});
	setUnreadMessagesChatWidget(withUnreadMessages != std::end(m_chatWidgets)
			? *withUnreadMessages
			: nullptr);
}

void ChatWidgetSetTitle::setUnreadMessagesChatWidget(ChatWidget *chatWidget)
{
	m_unreadMessagesChatWidget = chatWidget;
	if (m_unreadMessagesChatWidget)
		setCurrentChatWidget(m_unreadMessagesChatWidget);
	else
		setCurrentChatWidget(m_activeChatWidget);
}

void ChatWidgetSetTitle::setCurrentChatWidget(ChatWidget *chatWidget)
{
	if (m_currentChatWidget)
		disconnect(m_currentChatWidget->title(), SIGNAL(titleChanged(ChatWidget*)), this, SLOT(update()));
	m_currentChatWidget = chatWidget;
	if (m_currentChatWidget)
		connect(m_currentChatWidget->title(), SIGNAL(titleChanged(ChatWidget*)), this, SLOT(update()));

	update();
}

void ChatWidgetSetTitle::update()
{
	emit titleChanged();
}
