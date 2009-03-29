/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMPLE_CHAT_H
#define SIMPLE_CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "chat/chat.h"
#include "configuration/storable-object.h"
#include "contacts/contact-list.h"

class XmlConfigFile;

class SimpleChat : public Chat
{
	Contact CurrentContact;

public:
	SimpleChat(Account *parentAccount, Contact contact, QUuid uuid = QUuid());
	virtual ~SimpleChat();
	virtual ContactList currentContacts();
};

#endif // SIMPLE_CHAT_H
