/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SET_H
#define CONTACT_SET_H

#include <QtCore/QSet>

#include "buddies/buddy.h"

#include "exports.h"

class ContactList;
class Account;

class KADUAPI ContactSet : public QSet<Contact>
{

public:
	ContactSet();
	explicit ContactSet(Contact contact);

	ContactList toContactList() const;
	Account prefferedAccount();

};

#endif // CONTACT_SET_H
