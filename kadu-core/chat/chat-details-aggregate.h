/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_DETAILS_AGGREGATE_H
#define CHAT_DETAILS_AGGREGATE_H

#include "chat/chat-details.h"
#include "contacts/contact.h"
#include "exports.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatDetailsAggregate
 * @short Chat data that aggregates some chats into one.
 *
 * Class contains list of chat objects. It joins these chats into one chat.
 * This is usable to join all chats for contacts of one buddy into one chat.
 */
class KADUAPI ChatDetailsAggregate : public ChatDetails
{
	Q_OBJECT

	QVector<Chat> Chats;

public:
	explicit ChatDetailsAggregate(ChatShared *chatData);
	virtual ~ChatDetailsAggregate();

	virtual bool shouldStore();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const;
	virtual QString name() const;

	void setChats(const QVector<Chat> &chats);
	const QVector<Chat> & chats() const;

};

/**
 * @}
 */

#endif // CHAT_DETAILS_AGGREGATE_H
