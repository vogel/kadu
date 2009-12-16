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

bool IgnoredHelper::isIgnored(BuddySet buddies)
{
	if (1 == buddies.count())
	{
		return (*buddies.begin()).isIgnored();
	}
	else
	{
		// TODO: 0.6.6 implement
		// ConferenceManager::instance()->byContactList(senders)->isIgnored(true)
		return false;
	}
}

void IgnoredHelper::setIgnored(BuddySet buddies, bool ignored)
{
	if (1 == buddies.count())
	{
		Buddy c = (*buddies.begin());
		c.setIgnored(ignored);
	}
	else
	{
		// TODO: 0.6.6 implement
		// ConferenceManager::instance()->byContactList(senders)->isIgnored(true)
	}
}
