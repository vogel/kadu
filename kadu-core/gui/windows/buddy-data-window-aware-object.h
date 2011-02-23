/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDY_DATA_WINDOW_AWARE_OBJECT
#define BUDDY_DATA_WINDOW_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class BuddyDataWindow;

/**
 * @class BuddyDataWindowAwareObject
 * @author Piotr 'ultr' Dąbrowski
 * @short Class that gets informed about BuddyDataWindow creation/destruction.
 *
 * Each class that inherits from BuddyDataWindowAwareObject has to implement two methods:
 * buddyDataWindowCreated and buddyDataWindowDestroyed that are called automatically when
 * a BuddyDataWindow window is created or destroyed.
 */
class KADUAPI BuddyDataWindowAwareObject : public AwareObject<BuddyDataWindowAwareObject>
{
protected:
	/**
	* @author Piotr 'ultr' Dąbrowski
	* @short Method is called after new BuddyDataWindow window is created
	* @param buddydatawindow created window
	*
	* Method is called after a new BuddyDataWindow is created.
	* This method must be overridden.
	*/
	virtual void buddyDataWindowCreated(BuddyDataWindow *buddydatawindow) = 0;

	/**
	* @author Piotr 'ultr' Dąbrowski
	* @short Method is called when a BuddyDataWindow window is destroyed.
	* @param buddydatawindow destroyed window
	*
	* Method is called after a BuddyDataWindow is destroyed.
	* This method must be overridden.
	*/
	virtual void buddyDataWindowDestroyed(BuddyDataWindow *buddydatawindow) = 0;

public:
	static KADUAPI void notifyBuddyDataWindowCreated(BuddyDataWindow *buddydatawindow);
	static KADUAPI void notifyBuddyDataWindowDestroyed(BuddyDataWindow *buddydatawindow);

	void triggerAllBuddyDataWindowsCreated();
	void triggerAllBuddyDataWindowsDestroyed();

};

#endif // BUDDY_DATA_WINDOW_AWARE_OBJECT
