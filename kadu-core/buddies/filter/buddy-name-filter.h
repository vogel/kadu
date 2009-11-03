 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_NAME_FILTER_H
#define BUDDY_NAME_FILTER_H

#include "buddies/filter/abstract-buddy-filter.h"

class BuddyNameFilter : public AbstractBuddyFilter
{
	QString Name;

public:
	BuddyNameFilter(QObject *parent = 0) : AbstractBuddyFilter(parent) {}

	virtual bool acceptBuddy(Buddy buddy);

	void setName(const QString &name);

};

#endif // BUDDY_NAME_FILTER_H
