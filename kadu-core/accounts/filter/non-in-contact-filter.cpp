 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"

#include "non-in-contact-filter.h"

NonInContactFilter::NonInContactFilter(QObject *parent) :
		AbstractAccountFilter(parent), MyContact(Buddy::null)
{
}

NonInContactFilter::~NonInContactFilter()
{
}

bool NonInContactFilter::acceptAccount(Account account)
{
	return MyContact.isNull() || MyContact.contact(account).isNull();
}

void NonInContactFilter::setContact(Buddy buddy)
{
	if (MyContact != buddy)
	{
		MyContact = buddy;
		emit filterChanged();
	}
}
