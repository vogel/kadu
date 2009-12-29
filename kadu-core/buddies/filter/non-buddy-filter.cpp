/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "non-buddy-filter.h"

NonBuddyFilter::NonBuddyFilter(QObject *parent) :
		AbstractBuddyFilter(parent)
{
}

NonBuddyFilter::~NonBuddyFilter()
{
}

void NonBuddyFilter::setBuddy(Buddy buddy)
{
	if (CurrentBuddy != buddy)
	{
		CurrentBuddy = buddy;
		emit filterChanged();
	}
}

bool NonBuddyFilter::acceptBuddy(Buddy buddy)
{
	return buddy != CurrentBuddy;
}
