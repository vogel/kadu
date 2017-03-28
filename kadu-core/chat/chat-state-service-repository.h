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
#include "misc/iterator.h"

#include <QtCore/QObject>
#include <map>

class ChatStateService;

class KADUAPI ChatStateServiceRepository : public QObject
{
    Q_OBJECT

public:
    using Storage = std::map<Account, ChatStateService *>;
    using WrappedIterator = Storage::iterator;
    using Iterator = IteratorWrapper<WrappedIterator, ChatStateService *>;

    Q_INVOKABLE explicit ChatStateServiceRepository(QObject *parent = nullptr) : QObject{parent}
    {
    }
    virtual ~ChatStateServiceRepository() = default;

    Iterator begin();
    Iterator end();

    ChatStateService *chatStateService(const Account &account) const;

public slots:
    void addChatStateService(ChatStateService *chatStateService);
    void removeChatStateService(ChatStateService *chatStateService);

signals:
    void chatStateServiceAdded(ChatStateService *chatStateService);
    void chatStateServiceRemoved(ChatStateService *chatStateService);

private:
    Storage m_chatStateServices;
};

inline ChatStateServiceRepository::Iterator begin(ChatStateServiceRepository *chatStateServiceRepository)
{
    return chatStateServiceRepository->begin();
}

inline ChatStateServiceRepository::Iterator end(ChatStateServiceRepository *chatStateServiceRepository)
{
    return chatStateServiceRepository->end();
}
