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
#include "contacts/contact-set.h"

#include "chat/chat-details.h"

class ChatDetailsConference : public ChatDetails
{
	Q_OBJECT

	ContactSet Contacts;

protected:
	virtual void load();

public:
	explicit ChatDetailsConference(ChatShared *chatData);
	virtual ~ChatDetailsConference();

	virtual void store();
	virtual bool shouldStore();

	virtual ChatType * type() const;
	virtual ContactSet contacts() const { return Contacts; }
	virtual QString name() const;

	void setContacts(ContactSet contacts);

};

#endif // CHAT_DETAILS_CONFERENCE_H
