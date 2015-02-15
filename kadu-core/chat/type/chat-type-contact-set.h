/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_TYPE_CONTACT_SET_H
#define CHAT_TYPE_CONTACT_SET_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "chat/type/chat-type.h"
#include "storage/manager-common.h"

#include "exports.h"

class BuddySet;
class Chat;
class ContactSet;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class ChatTypeContactSet
 * @author Rafal 'Vogel' Malinowski
 * @short Representation of chat type with many buddies.
 *
 * Representation of chat type with many buddies.
 */
class KADUAPI ChatTypeContactSet : public ChatType
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeContactSet)

	QStringList Aliases;

public:
	static Chat findChat(const ContactSet &contacts, NotFoundAction notFoundAction);

	explicit ChatTypeContactSet(QObject *parent = 0);
	virtual ~ChatTypeContactSet();

	virtual QString name() const;
	virtual QStringList aliases() const;
	virtual KaduIcon icon() const;
	virtual QString windowRole() const;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const;
	virtual ChatEditWidget *createEditWidget(const Chat &chat, QWidget *parent) const;

};

Q_DECLARE_METATYPE(ChatTypeContactSet *)

/**
 * @}
 */

#endif // CHAT_TYPE_CONTACT_SET_H
