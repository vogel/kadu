/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_REMOVE_PREDICATE_OBJECT
#define BUDDY_REMOVE_PREDICATE_OBJECT

#include "predicate-object.h"

class Buddy;

class KADUAPI BuddyRemovePredicateObject : public PredicateObject<BuddyRemovePredicateObject>
{

protected:
	virtual bool removeContactFromStorage(Buddy contact) = 0;

public:
	static bool inquireAll(Buddy contact);

};

#endif // BUDDY_REMOVE_PREDICATE_OBJECT
