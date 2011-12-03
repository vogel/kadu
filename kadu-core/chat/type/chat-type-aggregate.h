/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_TYPE_AGGREGATE_H
#define CHAT_TYPE_AGGREGATE_H

#include <QtCore/QString>
#include <QtCore/QVariant>

#include "chat/type/chat-type.h"

#include "exports.h"

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeAggregate
 * @author Rafal 'Vogel' Malinowski
 * @short Representation of chat type that contains other chats.
 *
 * Representation of chat type that contains other chats.
 */
class KADUAPI ChatTypeAggregate : public ChatType
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeAggregate)

public:
	explicit ChatTypeAggregate(QObject *parent = 0);
	virtual ~ChatTypeAggregate();

	virtual int sortIndex() const;
	virtual QString name() const;
	virtual QString displayName() const;
	virtual QString displayNamePlural() const;
	virtual KaduIcon icon() const;
	virtual QString windowRole() const;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const;

};

Q_DECLARE_METATYPE(ChatTypeAggregate *)

/**
 * @}
 */

#endif // CHAT_TYPE_AGGREGATE_H
