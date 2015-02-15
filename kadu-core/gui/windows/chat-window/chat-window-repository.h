/*
 * %kadu copyright begin%
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

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <injeqt/injeqt.h>

#include "chat/chat.h"
#include "misc/iterator.h"
#include "exports.h"

class ChatWidget;
class ChatWindow;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWindowRepository
 * @short Repository of ChatWindow instances.
 *
 * This repository holds instances of ChatWindow class.
 *
 * Instances are added and removed by addChatWidget(ChatWindow*) and
 * removeChatWidget(ChatWindow*) methods.
 * Access for them is provided by windowForChatWidget(Chat) and windows() methods.
 *
 * ChatWindow is also automatically removed when it is destroyed.
 */
class KADUAPI ChatWindowRepository : public QObject
{
	Q_OBJECT

	using Storage = std::map<Chat, ChatWindow *>;
	using WrappedIterator = Storage::iterator;

public:
	using Iterator = IteratorWrapper<WrappedIterator, ChatWindow *>;

	Q_INVOKABLE explicit ChatWindowRepository(QObject *parent = nullptr);
	virtual ~ChatWindowRepository();

	/**
	 * @short Begin iterator that returns ChatWindow *.
	 */
	Iterator begin();

	/**
	 * @short Begin iterator that returns ChatWindow *.
	 */
	Iterator end();

	/**
	 * @short Return true if repository has chat window for given chat.
	 */
	bool hasWindowForChat(const Chat &chat) const;

	/**
	 * @short Return ChatWindow for given chatWidget.
	 * @param chat chat to get ChatWindow for
	 * @return ChatWindow for given chat
	 *
	 * If chat is null then nullptr is returned. If repository does contain chat then
	 * it is returned. Else nullptr is returned.
	 */
	ChatWindow * windowForChat(const Chat &chat);

public slots:
	/**
	 * @short Add new chatWindow to repository.
	 *
	 * Add new chatWindow to repository only if it is valid and not already in repository.
	 */
	void addChatWindow(ChatWindow *chatWindow);

	/**
	 * @short Remove chatWindow from repository.
	 *
	 * Remove chatWindow from repository only if it is  already in repository.
	 */
	void removeChatWindow(ChatWindow *chatWindow);

private:
	static ChatWindow * converter(WrappedIterator iterator);

	Storage m_windows;

};

inline ChatWindowRepository::Iterator begin(ChatWindowRepository *chatWindowRepository)
{
	return chatWindowRepository->begin();
}

inline ChatWindowRepository::Iterator end(ChatWindowRepository *chatWindowRepository)
{
	return chatWindowRepository->end();
}

/**
 * @}
 */
