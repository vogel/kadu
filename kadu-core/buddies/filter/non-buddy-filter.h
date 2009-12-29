/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NON_BUDDY_FILTER_H
#define NON_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "buddies/buddy.h"

#include "abstract-buddy-filter.h"

class NonBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	Buddy CurrentBuddy;

public:
	explicit NonBuddyFilter(QObject *parent = 0);
	virtual ~NonBuddyFilter();

	void setBuddy(Buddy buddy);
	virtual bool acceptBuddy(Buddy buddy);

};

Q_DECLARE_METATYPE(NonBuddyFilter *)

#endif // NON_BUDDY_FILTER_H
