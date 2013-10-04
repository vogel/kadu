/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-data-window-repository.h"

BuddyDataWindowRepository::BuddyDataWindowRepository(QObject *parent) :
		QObject(parent)
{
}

BuddyDataWindowRepository::~BuddyDataWindowRepository()
{
}


BuddyDataWindow * BuddyDataWindowRepository::windowForBuddy(const Buddy &buddy)
{
	if (Windows.contains(buddy))
		return Windows.value(buddy);

	if (buddy == Core::instance()->myself())
		return 0;

	BuddyDataWindow *result = new BuddyDataWindow(Core::instance()->buddyConfigurationWidgetFactoryRepository(), buddy);
	connect(result, SIGNAL(destroyed(Buddy)), this, SLOT(windowDestroyed(Buddy)));
	Windows.insert(buddy, result);

	return result;
}

const QMap<Buddy, BuddyDataWindow *> & BuddyDataWindowRepository::windows() const
{
	return Windows;
}

void BuddyDataWindowRepository::windowDestroyed(const Buddy &buddy)
{
	Windows.remove(buddy);
}

void BuddyDataWindowRepository::showBuddyWindow(const Buddy &buddy)
{
	BuddyDataWindow *window = windowForBuddy(buddy);
	if (window)
	{
		window->show();
		window->raise();
	}
}
