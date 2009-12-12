/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_AGGREGATOR_BUILDER_H
#define CHAT_AGGREGATOR_BUILDER_H

#include <QtCore/QObject>

#include "exports.h"

class Chat;
class BuddySet;

class KADUAPI ChatAggregatorBuilder : public QObject
{
	Q_OBJECT
public:
	ChatAggregatorBuilder();
	//ChatAggregatorBuilder(QList<Chat> chats);
	virtual ~ChatAggregatorBuilder();

	static Chat  buildAggregateChat(BuddySet buddies);
/*
	virtual void load();
	virtual void store();

	virtual ContactSet contacts() const { return Chats.at(0)->contacts(); }
	virtual QUuid uuid() const { return Chats.at(0)->uuid(); }

	Account account() { return Chats.at(0)->account(); }
	QString title() { return Chats.at(0)->title(); }
	QPixmap icon() { return Chats.at(0)->icon(); }

	QList<Chat> chats() const { return Chats; };
	void setChats(QList<Chat> chats) { Chats = chats; };

	void addChat(Chat chat);
	void removeChat(Chat chat);*/
};

#endif // AGGREGATE_CHAT_H
