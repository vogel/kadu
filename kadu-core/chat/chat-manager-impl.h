/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVector>
#include <injeqt/injeqt.h>

#include "chat/chat-manager.h"
#include "chat/chat.h"
#include "message/message.h"
#include "storage/manager.h"
#include "exports.h"

/**
 * @defgroup Chat
 *
 * Chats are basic concept in Kadu. Chat can define any conversation beetwen people. In core Kadu code
 * exists two kinds of chats:
 * <ul>
 *  <li>contact - chat with one person</li>
 *  <li>confernece - chat with more that one person</li>
 * </ul>
 *
 * Other kinds of chats can be defined (e.g. IRC chat would be defined by server and IRC room name,
 * not by the people that are in the conversation for a given moment).
 *
 * All currently defined kinds of chats are stored in @link ChatTypeManger @endlink class. Plugins
 * can add new kind of chats and register them in manager. Every object that needs to know about
 * all installed chat types should derive from @link ChatTypeAwareObject @endlink.
 *
 * @link ChatManager @endlink stores all chats that were used in program. Every chat type has associated
 * @link ChatDetails @endlink class that stored data specific to this chat type. ChatManager can load
 * only data common for all chats, remaining data can only by loaded by details class. Two details classes
 * are defined in core: @link ChatDetailsContact @endlink and @link ChatDetailsContactSet @endlink
 */

/**
 * @addtogroup Chat
 * @{
 */

class Account;
class BuddySet;
class ChatStorage;
class ConfigurationApi;
class ConfigurationManager;
class UnreadMessageRepository;

class KADUAPI ChatManagerImpl : public ChatManager
{
	Q_OBJECT

	QPointer<ChatStorage> m_chatStorage;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<UnreadMessageRepository> m_unreadMessageRepository;

private slots:
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setUnreadMessageRepository(UnreadMessageRepository *unreadMessageRepository);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void chatDataUpdated();
	void chatOpened();
	void chatClosed();

	void unreadMessageAdded(const Message &message);
	void unreadMessageRemoved(const Message &message);

protected:
	virtual Chat loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint) override;

	virtual void itemAboutToBeRegistered(Chat item) override;
	virtual void itemRegistered(Chat item) override;
	virtual void itemAboutToBeUnregisterd(Chat item) override;
	virtual void itemUnregistered(Chat item) override;

public:
	Q_INVOKABLE explicit ChatManagerImpl(QObject *parent = nullptr);
	virtual ~ChatManagerImpl();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name for storage of all chat data.
	 * @return node name for storage of all chat data
	 *
	 * Returns node name for storage of all chat data - "Chat".
	 */
	virtual QString storageNodeName() override { return QStringLiteral("Chats"); }

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name for storage of given chat data.
	 * @return node name for storage of given chat data
	 *
	 * Returns node name for storage of given chat data - "Chat".
	 */
	virtual QString storageNodeItemName() override { return QStringLiteral("Chat"); }

	virtual QVector<Chat> chats(const Account &account) override;
	virtual Chat byDisplay(const QString &display) override;

};

/**
 * @}
 */
