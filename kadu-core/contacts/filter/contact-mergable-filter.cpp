/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact-mergable-filter.h"

ContactMergableFilter::ContactMergableFilter(Contact contact, QObject *parent) :
		AbstractContactFilter(parent), MyContact(contact)
{
	Accounts = MyContact.accounts().toSet();
}

ContactMergableFilter::~ContactMergableFilter()
{
}

void ContactMergableFilter::setContact(Contact contact)
{
	if (MyContact == contact)
		return;

	MyContact = contact;
	Accounts = contact.accounts().toSet();

	emit filterChanged();
}

bool ContactMergableFilter::acceptContact(Contact contact)
{
	return contact.accounts().toSet().intersect(Accounts).empty();
}
