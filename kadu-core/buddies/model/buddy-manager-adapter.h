/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_MANAGER_ADAPTER_H
#define BUDDY_MANAGER_ADAPTER_H

#include <QtCore/QObject>

#include "buddies/buddy.h"
#include "contacts/contact.h"

class BuddyListModel;

class BuddyManagerAdapter : public QObject
{
	Q_OBJECT

	BuddyListModel *Model;
	bool IncludeMyself;

private slots:
	void buddyAdded(const Buddy &buddy);
	void buddyRemoved(const Buddy &buddy);

public:
	explicit BuddyManagerAdapter(BuddyListModel *model);
	virtual ~BuddyManagerAdapter();

	void setIncludeMyself(bool includeMyself);
	bool includeMyself() const { return IncludeMyself; }

};

#endif // BUDDY_MANAGER_ADAPTER_H
