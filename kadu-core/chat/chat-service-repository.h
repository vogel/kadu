/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "exports.h"

#include <QtCore/QObject>
#include <map>

class ChatService;

class KADUAPI ChatServiceRepository : public QObject
{
	Q_OBJECT

	using Storage = std::map<Account, ChatService *>;

public:
	Q_INVOKABLE explicit ChatServiceRepository(QObject *parent = nullptr) : QObject{parent} {}
	virtual ~ChatServiceRepository() = default;

	ChatService * chatService(const Account &account) const;

public slots:
	void addChatService(const Account &account, ChatService *chatService);
	void removeChatService(const Account &account);

signals:
	void chatServiceAdded(const Account &account, ChatService *chatService);
	void chatServiceRemoved(const Account &account, ChatService *chatService);

private:
	Storage m_chatServices;

};
