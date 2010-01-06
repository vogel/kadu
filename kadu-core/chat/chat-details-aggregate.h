/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_DETAILS_AGGREGATE_H
#define CHAT_DETAILS_AGGREGATE_H

#include "contacts/contact.h"

#include "chat/chat-details.h"

class ChatDetailsAggregate : public ChatDetails
{
	Q_OBJECT

	QList<Chat> Chats;

public:
	explicit ChatDetailsAggregate(ChatShared *chatData);
	virtual ~ChatDetailsAggregate();

	virtual bool shouldStore();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const;
	virtual QString name() const;

	void setChats(QList<Chat> chats);
	QList<Chat> chats();

};

#endif // CHAT_DETAILS_AGGREGATE_H
