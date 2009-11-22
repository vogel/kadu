/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_DETAILS_CONFERENCE_H
#define CHAT_DETAILS_CONFERENCE_H

#include "buddies/buddy-set.h"
#include "contacts/contact.h"

#include "chat/chat-details.h"

class ChatDetailsConference : public ChatDetails
{
	Q_OBJECT

	BuddySet Buddies;

public:
	explicit ChatDetailsConference(Chat *chat);
	virtual ~ChatDetailsConference();

	virtual void load();
	virtual void store();

	virtual ChatType * type() const;
	virtual BuddySet buddies() const { return Buddies; }
	virtual QString name() const;

	void setBuddies(BuddySet buddies);

};

#endif // CHAT_DETAILS_CONFERENCE_H
