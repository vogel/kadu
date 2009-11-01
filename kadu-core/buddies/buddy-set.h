/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_SET_H
#define BUDDY_SET_H

#include <QtCore/QSet>

#include "buddies/buddy.h"

#include "exports.h"

class BuddyList;
class Account;

class KADUAPI BuddySet : public QSet<Buddy>
{

public:
	BuddySet();
	explicit BuddySet(Buddy contact);

	BuddyList toBuddyList() const;
	Account prefferedAccount();

};

#endif // BUDDY_SET_H
