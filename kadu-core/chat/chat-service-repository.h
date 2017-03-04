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
#include "misc/iterator.h"
#include "exports.h"

#include <QtCore/QObject>
#include <map>

class ChatService;

class KADUAPI ChatServiceRepository : public QObject
{
	Q_OBJECT

public:
	using Storage = std::map<Account, ChatService *>;
	using WrappedIterator = Storage::iterator;
	using Iterator = IteratorWrapper<WrappedIterator, ChatService *>;

	Q_INVOKABLE explicit ChatServiceRepository(QObject *parent = nullptr) : QObject{parent} {}
	virtual ~ChatServiceRepository() = default;

	Iterator begin();
	Iterator end();

	ChatService * chatService(const Account &account) const;

public slots:
	void addChatService(ChatService *chatService);
	void removeChatService(ChatService *chatService);

signals:
	void chatServiceAdded(ChatService *chatService);
	void chatServiceRemoved(ChatService *chatService);

private:
	Storage m_chatServices;

};

inline ChatServiceRepository::Iterator begin(ChatServiceRepository *chatWidgetRepository)
{
	return chatWidgetRepository->begin();
}

inline ChatServiceRepository::Iterator end(ChatServiceRepository *chatWidgetRepository)
{
	return chatWidgetRepository->end();
}
