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

#include "window-chat-widget-container-handler.h"

#include "activate.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/chat-window/chat-window.h"
#include "gui/windows/chat-window/chat-window-factory.h"
#include "gui/windows/chat-window/chat-window-repository.h"

WindowChatWidgetContainerHandler::WindowChatWidgetContainerHandler(QObject *parent) :
		ChatWidgetContainerHandler{parent}
{
}

WindowChatWidgetContainerHandler::~WindowChatWidgetContainerHandler()
{
}

void WindowChatWidgetContainerHandler::setChatWindowFactory(ChatWindowFactory *chatWindowFactory)
{
	m_chatWindowFactory = chatWindowFactory;
}

void WindowChatWidgetContainerHandler::setChatWindowRepository(ChatWindowRepository *chatWindowRepository)
{
	m_chatWindowRepository = chatWindowRepository;
}

bool WindowChatWidgetContainerHandler::containChatWidget(ChatWidget *chatWidget)
{
	if (!m_chatWindowFactory || !m_chatWindowRepository)
		return false;

	auto chatWindow = m_chatWindowRepository.data()->windowForChatWidget(chatWidget);
	if (!chatWindow)
	{
		chatWindow = m_chatWindowFactory.data()->createChatWindow(chatWidget).release();
		if (!chatWindow)
			return false;

		m_chatWindowRepository.data()->addChatWindow(chatWindow);
	}

	chatWidget->setContainer(chatWindow);
	chatWindow->show();

	return true;
}

bool WindowChatWidgetContainerHandler::isChatWidgetActive(ChatWidget *chatWidget)
{
	if (!chatWidget || !m_chatWindowRepository)
		return false;

	auto chatWindow = m_chatWindowRepository.data()->windowForChatWidget(chatWidget);
	return chatWindow ? _isWindowActiveOrFullyVisible(chatWindow) : false;
}
