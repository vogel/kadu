/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "gui/widgets/chat-widget/chat-widget-container-handler.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWindow;
class ChatWindowFactory;
class ChatWindowRepository;

class WindowChatWidgetContainerHandler : public ChatWidgetContainerHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit WindowChatWidgetContainerHandler(QObject *parent = nullptr);
	virtual ~WindowChatWidgetContainerHandler();

	virtual bool acceptChat(Chat chat) const override;
	virtual ChatWidget * addChat(Chat chat, OpenChatActivation activation) override;
	virtual void removeChat(Chat chat) override;

	virtual bool isChatWidgetActive(ChatWidget *chatWidget) override;
	virtual void tryActivateChatWidget(ChatWidget *chatWidget) override;
    virtual void tryMinimizeChatWidget(ChatWidget *chatWidget) override;

private:
	QPointer<ChatWindowFactory> m_chatWindowFactory;
	QPointer<ChatWindowRepository> m_chatWindowRepository;

private slots:
	INJEQT_SETTER void setChatWindowFactory(ChatWindowFactory *chatWindowFactory);
	INJEQT_SETTER void setChatWindowRepository(ChatWindowRepository *chatWindowRepository);

	void chatWindowActivated(ChatWindow *chatWindow);

};
