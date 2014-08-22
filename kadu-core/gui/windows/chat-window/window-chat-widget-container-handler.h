/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

class ChatWindow;
class ChatWindowFactory;
class ChatWindowRepository;

class WindowChatWidgetContainerHandler : public ChatWidgetContainerHandler
{
	Q_OBJECT

public:
	explicit WindowChatWidgetContainerHandler(QObject *parent = nullptr);
	virtual ~WindowChatWidgetContainerHandler();

	void setChatWindowFactory(ChatWindowFactory *chatWindowFactory);
	void setChatWindowRepository(ChatWindowRepository *chatWindowRepository);

	virtual bool acceptChatWidget(ChatWidget *chatWidget) const override;
	virtual void addChatWidget(ChatWidget *chatWidget) override;
	virtual void removeChatWidget(ChatWidget *chatWidget) override;

	virtual bool isChatWidgetActive(ChatWidget *chatWidget) override;
	virtual void tryActivateChatWidget(ChatWidget *chatWidget) override;

private:
	QPointer<ChatWindowFactory> m_chatWindowFactory;
	QPointer<ChatWindowRepository> m_chatWindowRepository;

private slots:
	void chatWindowActivated(ChatWindow *chatWindow);

};
