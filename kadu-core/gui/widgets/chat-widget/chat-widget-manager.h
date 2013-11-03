/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "buddies/buddy.h"
#include "message/message.h"

#include "exports.h"

class ChatWidget;
class ChatWidgetActions;
class ChatWidgetRepository;
class ChatWindowStorage;
class ChatWindowRepository;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetManager
 * @short Manager of all ChatWidget instances.
 * @todo replace handleNewChatWidget with better mechanism
 * @todo create some kind of per-container manager than can store its own configuration
 *
 * This singleton is responsible for all ChatWidget instances in Kadu.
 */
class KADUAPI ChatWidgetManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

public:
	/**
	 * @short Returns manager's singleton instance.
	 * @return manager's singleton instance
	 *
	 * Returns manager's singleton instance.
	 */
	static ChatWidgetManager * instance();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
	void setChatWindowRepository(ChatWindowRepository *chatWindowRepository);
	void setChatWindowStorage(ChatWindowStorage *chatWindowStorage);

	ChatWidgetActions * actions() { return m_actions; }

	/**
	 * @short Returns ChatWidget for given chat.
	 * @return ChatWidget for given chat
	 * @param chat chat for returned ChatWidget
	 * @param create if set to true new widget will be created if one does not exists yet
	 *
	 * Returns ChatWidget for given chat. If no ChatWidget exists for particular chat and create is false,
	 * 0 is returned. If create is true, new ChatWidget will be created (or 0, if chat is null).
	 *
	 * Newly created ChatWidget will have all pending messages loaded and will be put in a StatusWidgetContainer
	 * implementation.
	 */
	ChatWidget * byChat(const Chat &chat, const bool create);

public slots:
	void openChat(const Chat &chat);

	/**
	 * @short Close ChatWidget for given chat.
	 * @param chat chat to close chat widget for
	 *
	 * This method closes ChatWidget for given chat.
	 */
	void closeChat(const Chat &chat);

	/**
	 * @short Close ChatWidget for given buddy.
	 * @param buddy buddy to close chat widget for
	 *
	 * This method closes ChatWidget for all chats that contains only this buddy.
	 */
	void closeAllChats(const Buddy &buddy);

	/**
	 * @short Close all ChatWindow windows.
	 * @todo move to ChatWindowManager or something like that
	 *
	 * This method closes all ChatWindow windows.
	 */
	void closeAllWindows();

signals:
	/**
	 * @short Signal emited every time a new ChatWidget is created.
	 * @param chatWidget new ChatWidget
	 * @param handled receiver should set this to true if ChatWidgetContainer was created for given ChatWidget
	 * @todo refactor
	 *
	 * This signal is emited every time a new ChatWidget is created. A received can create ChatWidgetContainer and
	 * take over this ChatWidget. If so, handled should be set to true. If not, ChatWidgetManager will create
	 * default ChatWidgetContainer in form of ChatWindow.
	 */
	void handleNewChatWidget(ChatWidget *chatWidget, bool &handled);

private:
	static ChatWidgetManager *m_instance;

	QWeakPointer<ChatWidgetRepository> m_chatWidgetRepository;
	QWeakPointer<ChatWindowStorage> m_chatWindowStorage;
	QWeakPointer<ChatWindowRepository> m_chatWindowRepository;

	ChatWidgetActions *m_actions;
	bool m_persistedChatWindowsOpened;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

	void openPersistedChatWindows();

private slots:
	void chatWidgetCreated(ChatWidget *chatWidget);

};

/**
 * @}
 */
