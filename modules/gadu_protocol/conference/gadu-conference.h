/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONFERENCE_H
#define GADU_CONFERENCE_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "accounts/account.h"

#include "contacts/contact-list.h"

#include "conference/conference.h"

class GaduConference : public Conference
{
	ContactList Contacts;

public:
	GaduConference(Account *parentAccount, ContactList contacts, QUuid uuid = QUuid());
	virtual ~GaduConference();

	ContactList contacts() { return Contacts; }

	virtual void loadConfiguration();
	virtual void storeConfiguration();

};

#endif // GADU_CONFERENCE_H

