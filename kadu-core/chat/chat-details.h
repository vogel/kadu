/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_DETAILS_H
#define CHAT_DETAILS_H

#include "storage/storable-object.h"

class BuddySet;
class Chat;
class ChatShared;
class ChatType;
class ContactSet;

class ChatDetails : public QObject, public StorableObject
{
	Q_OBJECT

	ChatShared *ChatData;

public:
	explicit ChatDetails(ChatShared *chat);
	virtual ~ChatDetails();

	ChatShared * chatData() { return ChatData; }

	virtual ChatType * type() const = 0;
	virtual ContactSet contacts() const = 0;
	virtual QString name() const = 0;

signals:
	void titleChanged(Chat chat, const QString &newTitle);

};

#include "chat/chat.h" // for MOC

#endif // CHAT_DETAILS_H
