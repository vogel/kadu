/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_AWARE_OBJECT
#define BUDDIES_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class Buddy;

class KADUAPI BuddiesAwareObject : public AwareObject<BuddiesAwareObject>
{

protected:
	virtual void contactAdded(Buddy contact) = 0;
	virtual void contactRemoved(Buddy contact) = 0;

public:
	static void notifyBuddyAdded(Buddy contact);
	static void notifyBuddyRemoved(Buddy contact);

	void triggerAllBuddiesAdded();
	void triggerAllBuddiesRemoved();

};

#endif // BUDDIES_AWARE_OBJECT
