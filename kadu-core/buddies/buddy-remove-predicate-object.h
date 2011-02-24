/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef BUDDY_REMOVE_PREDICATE_OBJECT
#define BUDDY_REMOVE_PREDICATE_OBJECT

#include "predicate-object.h"

class Buddy;

class KADUAPI BuddyRemovePredicateObject : public PredicateObject<BuddyRemovePredicateObject>
{

protected:
	virtual bool removeContactFromStorage(Buddy buddy) = 0;

public:
	static KADUAPI bool inquireAll(Buddy buddy);

};

#endif // BUDDY_REMOVE_PREDICATE_OBJECT
