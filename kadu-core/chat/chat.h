/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"
#include "configuration/uuid-storable-object.h"
#include "contacts/contact-set.h"

class Chat : public UuidStorableObject
{
	Account *CurrentAccount;
	QUuid Uuid;

public:
	static Chat * loadFromStorage(StoragePoint *conferenceStoragePoint);

	explicit Chat(StoragePoint *storage);
	explicit Chat(Account *parentAccount, QUuid uuid = QUuid());
	virtual ~Chat();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }

	virtual ContactSet contacts() = 0;
	Account * account() { return CurrentAccount; }

};

#endif // CHAT_H
