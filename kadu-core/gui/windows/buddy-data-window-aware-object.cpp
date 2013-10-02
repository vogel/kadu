/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "core/core.h"
#include "gui/windows/buddy-data-window.h"
#include "gui/windows/buddy-data-window-repository.h"

#include "buddy-data-window-aware-object.h"

KADU_AWARE_CLASS(BuddyDataWindowAwareObject)

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Call this method to call buddyDataWindowCreated in each BuddyDataWindowAwareObject object.
 * @param buddydatawindow BuddyDataWindow widget that will get passed to buddyDataWindowCreated methods
 *
 * Calling this method results in calling buddyDataWindowCreated in each BuddyDataWindowAwareObject
 * in system.
 */
void BuddyDataWindowAwareObject::notifyBuddyDataWindowCreated(BuddyDataWindow *buddydatawindow)
{
	foreach (BuddyDataWindowAwareObject *object, Objects)
		object->buddyDataWindowCreated(buddydatawindow);
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Call this method to call buddyDataWindowDestroyed in each BuddyDataWindowAwareObject object.
 * @param buddydatawindow BuddyDataWindow widget that will get passed to buddyDataWindowDestroyed methods
 *
 * Calling this method results in calling buddyDataWindowDestroyed in each BuddyDataWindowAwareObject
 * in system.
 */
void BuddyDataWindowAwareObject::notifyBuddyDataWindowDestroyed(BuddyDataWindow *buddydatawindow)
{
	foreach (BuddyDataWindowAwareObject *object, Objects)
		object->buddyDataWindowDestroyed(buddydatawindow);
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Call this method to call buddyDataWindowCreated for each existing BuddyDataWindow.
 *
 * Calling this method results in calling buddyDataWindowCreated for each existing BuddyDataWindow
 * on current object.
 */
void BuddyDataWindowAwareObject::triggerAllBuddyDataWindowsCreated()
{
	foreach (BuddyDataWindow *buddydatawindow, Core::instance()->buddyDataWindowRepository()->windows())
		buddyDataWindowCreated(buddydatawindow);
}

/**
 * @author Piotr 'ultr' Dąbrowski
 * @short Call this method to call buddyDataWindowDestroyed for each existing BuddyDataWindow.
 *
 * Calling this method results in calling buddyDataWindowDestroyed for each existing BuddyDataWindow
 * on current object.
 */
void BuddyDataWindowAwareObject::triggerAllBuddyDataWindowsDestroyed()
{
	foreach (BuddyDataWindow *buddydatawindow, Core::instance()->buddyDataWindowRepository()->windows())
		buddyDataWindowDestroyed(buddydatawindow);
}
