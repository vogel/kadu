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

#include "single-window-chat-widget-container-handler.h"
#include "single-window-chat-widget-container-handler.moc"

#include "single-window.h"

#include "widgets/chat-widget/chat-widget-container-handler-repository.h"

SingleWindowChatWidgetContainerHandler::SingleWindowChatWidgetContainerHandler(QObject *parent)
        : ChatWidgetContainerHandler(parent)
{
}

SingleWindowChatWidgetContainerHandler::~SingleWindowChatWidgetContainerHandler()
{
}

void SingleWindowChatWidgetContainerHandler::setChatWidgetContainerHandlerRepository(
    ChatWidgetContainerHandlerRepository *chatWidgetContainerHandlerRepository)
{
    m_chatWidgetContainerHandlerRepository = chatWidgetContainerHandlerRepository;
}

void SingleWindowChatWidgetContainerHandler::setSingleWindow(SingleWindow *singleWindow)
{
    m_singleWindow = singleWindow;

    connect(
        m_singleWindow.data(), SIGNAL(chatWidgetActivated(ChatWidget *)), this,
        SIGNAL(chatWidgetActivated(ChatWidget *)));
}

void SingleWindowChatWidgetContainerHandler::init()
{
    m_chatWidgetContainerHandlerRepository->registerChatWidgetContainerHandler(this);
}

void SingleWindowChatWidgetContainerHandler::done()
{
    m_chatWidgetContainerHandlerRepository->unregisterChatWidgetContainerHandler(this);
}

bool SingleWindowChatWidgetContainerHandler::acceptChat(Chat chat) const
{
    Q_UNUSED(chat);

    return m_singleWindow != nullptr;
}

ChatWidget *SingleWindowChatWidgetContainerHandler::addChat(Chat chat, OpenChatActivation activation)
{
    if (!chat || !m_singleWindow)
        return nullptr;

    return m_singleWindow.data()->addChat(chat, activation);
}

void SingleWindowChatWidgetContainerHandler::removeChat(Chat chat)
{
    if (!chat || !m_singleWindow)
        return;

    m_singleWindow.data()->removeChat(chat);
}

bool SingleWindowChatWidgetContainerHandler::isChatWidgetActive(ChatWidget *chatWidget)
{
    return m_singleWindow ? m_singleWindow.data()->isChatWidgetActive(chatWidget) : false;
}

void SingleWindowChatWidgetContainerHandler::tryActivateChatWidget(ChatWidget *chatWidget)
{
    if (m_singleWindow)
        m_singleWindow.data()->tryActivateChatWidget(chatWidget);
}

void SingleWindowChatWidgetContainerHandler::tryMinimizeChatWidget(ChatWidget *)
{
}
