/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AGGREGATE_CHAT_H
#define AGGREGATE_CHAT_H

#include "chat/type/chat-type.h"
#include "chat/chat.h"

class XmlConfigFile;

class AggregateChat : public Chat
{
	QList<Chat *> Chats;

public:
	AggregateChat(Chat *chat);
	AggregateChat(QList<Chat *> chats);
	virtual ~AggregateChat();

	virtual void load();
	virtual void store();

	virtual ContactSet contacts() const { return Chats.at(0)->contacts(); }
	virtual QUuid uuid() const { return Chats.at(0)->uuid(); }
	virtual ChatType type() const { return Chats.at(0)->type(); };
	virtual QString name() const { return Chats.at(0)->name(); };

	Account * account() { return Chats.at(0)->account(); }
	QString title() { return Chats.at(0)->title(); }
	QPixmap icon() { return Chats.at(0)->icon(); }

	QList<Chat *> chats() const { return Chats; };
	void setChats(QList<Chat *> chats) { Chats = chats; };

	void addChat(Chat *chat);
	void removeChat(Chat *chat);
};

#endif // AGGREGATE_CHAT_H
