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

#pragma once

#include "exports.h"
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatManager;
class Chat;
class MessageManager;
class Message;
class RecentChatRepository;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class RecentChatService
 * @short Service to keep RecentChatRepository in sync with configuration and messages.
 */
class KADUAPI RecentChatService : public QObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(SERVICE)

public:
	static const QString LAST_MESSAGE_DATE_TIME_PROPERTY;
	static const int KEEP_RECENT_FOR_SECONDS { 60 * 60 * 4 };

	Q_INVOKABLE explicit RecentChatService(QObject *parent = nullptr);
	virtual ~RecentChatService();

	void cleanUp();

private:
	QPointer<ChatManager> m_chatManager;
	QPointer<MessageManager> m_messageManager;
	QPointer<RecentChatRepository> m_recentChatRepository;

	void add(Chat chat) const;
	void remove(Chat chat) const;
	void update(Chat chat) const;
	bool isRecent(Chat chat) const;
	bool isRecent(QDateTime dateTime) const;
	bool isAlreadyInRepository(Chat chat) const;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setOpenChatRepository(RecentChatRepository *recentChatRepository);
	INJEQT_INIT void init();

	void message(const Message &message) const;
	void chatAdded(Chat chat) const;
	void chatRemoved(Chat chat) const;

};

/**
 * @}
 */
