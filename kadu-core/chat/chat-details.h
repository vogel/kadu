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

#include "chat/chat-shared.h"
#include "storage/details.h"

class BuddySet;
class Chat;
class ChatType;
class ContactSet;

class ChatDetails : public QObject, public Details<ChatShared>
{
	Q_OBJECT

public:
	explicit ChatDetails(ChatShared *mainData) : Details<ChatShared>(mainData) {}
	virtual ~ChatDetails() {}

	virtual ChatType * type() const = 0;
	virtual ContactSet contacts() const = 0;
	virtual QString name() const = 0;

signals:
	void titleChanged(Chat chat, const QString &newTitle);

};

#include "chat/chat.h" // for MOC

#endif // CHAT_DETAILS_H
