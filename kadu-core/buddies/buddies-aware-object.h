/*
 * %kadu copyright begin%
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

#ifndef BUDDIES_AWARE_OBJECT_H
#define BUDDIES_AWARE_OBJECT_H

#include <QtCore/QList>

#include "aware-object.h"

class Buddy;

class KADUAPI BuddiesAwareObject : public AwareObject<BuddiesAwareObject>
{

protected:
	virtual void contactAdded(Buddy buddy) = 0;
	virtual void contactRemoved(Buddy buddy) = 0;

public:
	static KADUAPI void notifyBuddyAdded(Buddy buddy);
	static KADUAPI void notifyBuddyRemoved(Buddy buddy);

	void triggerAllBuddiesAdded();
	void triggerAllBuddiesRemoved();

};

#endif // BUDDIES_AWARE_OBJECT_H
