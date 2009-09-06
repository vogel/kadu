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

#include "chat/chat.h"

class XmlConfigFile;

class SimpleChat : public Chat
{
	Contact CurrentContact;

public:
	SimpleChat(StoragePoint *storage);
	SimpleChat(Account *parentAccount, Contact contact, QUuid uuid = QUuid());
	virtual ~SimpleChat();

	virtual void load();
	virtual void store();

	virtual ChatType type() const;

	virtual ContactSet contacts() const;

};

#endif // SIMPLE_CHAT_H
