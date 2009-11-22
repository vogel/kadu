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

#include "configuration/storable-object.h"

class BuddySet;
class Chat;
class ChatType;

class ChatDetails : public QObject, public StorableObject
{
	Q_OBJECT

	Chat *CurrentChat;

public:
	explicit ChatDetails(Chat *chat);
	virtual ~ChatDetails();

	Chat * chat() { return CurrentChat; }

	virtual ChatType * type() const = 0;
	virtual BuddySet buddies() const = 0;
	virtual QString name() const = 0;

signals:
	void titleChanged(Chat *chat, const QString &newTitle);

};

#endif // CHAT_DETAILS_H
