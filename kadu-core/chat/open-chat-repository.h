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

#include "chat/chat.h"
#include "exports.h"

#include <QtCore/QObject>
#include <injeqt/injeqt.h>
#include <vector>

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class OpenChatRepository
 * @short Repository keep all Chat instances that are currently open
 */
class KADUAPI OpenChatRepository : public QObject
{
	Q_OBJECT

	using Storage = std::vector<Chat>;

public:
	using Iterator = Storage::const_iterator;

	Q_INVOKABLE explicit OpenChatRepository(QObject *parent = nullptr);
	virtual ~OpenChatRepository();

	Iterator begin() const;
	Iterator end() const;
	size_t size() const;

public slots:
	void addOpenChat(Chat chat);
	void removeOpenChat(Chat chat);

signals:
	void openChatAdded(Chat chat);
	void openChatRemoved(Chat chat);

private:
	Storage m_chats;

};

inline OpenChatRepository::Iterator begin(OpenChatRepository *openChatRepository)
{
	return openChatRepository->begin();
}

inline OpenChatRepository::Iterator end(OpenChatRepository *openChatRepository)
{
	return openChatRepository->end();
}

/**
 * @}
 */
