/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "buddies/buddy.h"
#include "buddies/buddy-set.h"

#include "ignored-helper.h"

bool IgnoredHelper::isIgnored(BuddySet contacts)
{
	if (1 == contacts.count())
	{
		return (*contacts.begin()).isIgnored();
	}
	else
	{
		// TODO: 0.6.6 implement
		// ConferenceManager::instance()->byContactList(senders)->isIgnored(true)
	}
}

void IgnoredHelper::setIgnored(BuddySet contacts, bool ignored)
{
	if (1 == contacts.count())
	{
		Buddy c = (*contacts.begin());
		c.setIgnored(ignored);
	}
	else
	{
		// TODO: 0.6.6 implement
		// ConferenceManager::instance()->byContactList(senders)->isIgnored(true)
	}
}
