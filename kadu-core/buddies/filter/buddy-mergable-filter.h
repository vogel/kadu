/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_MERGABLE_FILTER_H
#define BUDDY_MERGABLE_FILTER_H

#include "buddies/buddy.h"

#include "buddies/filter/abstract-buddy-filter.h"

class BuddyMergableFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	QSet<Account> Accounts;
	Buddy MyContact;

public:
	explicit BuddyMergableFilter(Buddy contact, QObject *parent = 0);
	virtual ~BuddyMergableFilter();

	virtual bool acceptBuddy(Buddy contact);

	void setContact(Buddy contact);

};

#endif // BUDDY_MERGABLE_FILTER_H
