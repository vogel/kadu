/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_WIDGET_MANAGER_H
#define CHAT_WIDGET_MANAGER_H

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "storage/storable-string-list.h"

#include "exports.h"

class ChatWidget;
class ChatWidgetActions;
class Protocol;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetManager
 * @short Manager of all ChatWidget instances.
 * @todo replace handleNewChatWidget with better mechanism
 * @todo create some kind of per-container manager than can store its own configuration
 * @todo remove StorableStringList inheritance
 *
 * This singleton is responsible for all ChatWidget instances in Kadu.
 */
class KADUAPI ChatWidgetManager : public QObject, ConfigurationAwareObject, StorableStringList
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

	static ChatWidgetManager *Instance;

	ChatWidgetActions *Actions;

	QHash<Chat, ChatWidget *> Chats;

	bool AutoRaise;
	bool OpenChatOnMessage;
	bool OpenChatOnMessageWhenOnline;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

	/**
	 * @short Creates new instance of ChatWidget for given chat.
	 * @param chat chat that will be associated with new ChatWidget
	 * @return new ChatWidget instance for given chat.
	 * @todo refactor, simplify - reading pending messages should be out of scope of this class
	 *
	 * This method creates new instance of ChatWidget for given chat. Signal handleNewChatWidget is emited
	 * to find ChatWidgetContainer for this new object. Content of PendingMessages for given chat is loaded
	 * into widget.
	 */
	ChatWidget * createChatWidget(const Chat &chat);

	/**
	 * @todo remove - reading pending messages should be out of scope of this class
	 */
	QList<Message> loadUnreadMessages(const Chat &chat);

	bool shouldOpenChatWidget(const Message &message);

private slots:
	/**
	 * @short Slot called when ChatWidget is destroyed.
	 *
	 * Slot called when ChatWidget is destroyed. Removes mapping Chat-ChatWidget map entry for destroyed
	 * widget. Emits chatWidgetDestroying signal.
	 */
	void chatWidgetDestroyed();

	/**
	 * @short Slot called when message is sent from Kadu.
	 * @param message sent message
	 *
	 * This slot is called everytime a message is sent from Kadu. It opens chat widget if it is not opened
	 * already, and then puts new message in this window.
	 *
	 * In multilogon protocols this will automatically open new chat widget when a chat is started on one of
	 * other instances of IM with the same account logged.
	 */
	void messageSent(const Message &message);

protected:
	virtual void load();
	virtual void store();

	virtual void configurationUpdated();

public:
	/**
	 * @short Returns manager's singleton instance.
	 * @return manager's singleton instance
	 *
	 * Returns manager's singleton instance.
	 */
	static ChatWidgetManager * instance();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	ChatWidgetActions * actions() { return Actions; }

	/**
	 * @short Returns Chat-ChatWidget map.
	 * @return Chat-ChatWidget map
	 *
	 * Returns Chat-ChatWidget map.
	 */
	const QHash<Chat, ChatWidget *> & chats() const;

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
	/**
	 * @short Slot called when new message is received.
	 * @param message received message
	 * @todo make private again or move somewhere else, it is only required by imagelink plugin
	 * @todo best option - move to new MessageManager class
	 *
	 * This slot is called every time a new message is received. New message is added to ChatWidget, if it is
	 * already opened. If not, action depends on user configuration - new ChatWidget can be opened or message
	 * will be added to list of pending messages.
	 */
	void messageReceived(const Message &message);

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

	/**
	 * @short Signal emited every time a new ChatWidget is created.
	 * @param chatWidget new ChatWidget
	 *
	 * This signal is emited every time a new ChatWidget is created.
	 */
	void chatWidgetCreated(ChatWidget *chatWidget);

	/**
	 * @short Signal emited every time a ChatWidget is about to be destroyed.
	 * @param chatWidget ChatWidget that is about to be destroyed
	 *
	 * This signal emited every time a ChatWidget is about to be destroyed.
	 */
	void chatWidgetDestroying(ChatWidget *chatWidget);

};

/**
 * @}
 */

#endif // CHAT_WIDGET_MANAGER_H
