/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SET_H
#define CONTACT_SET

#include <QtCore/QSet>

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "contacts/contact.h"

#include "exports.h"

class Contact;

class KADUAPI ContactSet : public QSet<Contact>
{

public:
	ContactSet();
	explicit ContactSet(Contact contact);

	QList<Contact> toContactList() const;
	BuddySet toBuddySet() const;
	Contact firstContact() const;
	Account account() const;

};

#endif // BUDDY_SET_H
