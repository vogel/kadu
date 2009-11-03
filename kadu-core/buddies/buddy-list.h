/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_LIST_H
#define BUDDY_LIST_H

#include <QtCore/QList>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI BuddyList : public QList<Buddy>
{

public:

	BuddyList();
	BuddyList(QList<Buddy> list);
	BuddyList(Buddy buddy);

	bool operator == (const BuddyList &compare) const;

};

#endif // BUDDY_LIST_H
