/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONTACT_SET_H
#define CONTACT_SET_H

#include <QtCore/QSet>

#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "contacts/contact.h"

#include "exports.h"

class Contact;
class BuddySet;

class KADUAPI ContactSet : public QSet<Contact>
{

public:
	ContactSet();
	explicit ContactSet(const Contact &contact);

	QVector<Contact> toContactVector() const;
	BuddySet toBuddySet() const;
	Contact toContact() const;

};

#endif // CONTACT_SET_H
