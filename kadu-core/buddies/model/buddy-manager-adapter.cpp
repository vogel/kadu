/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"

#include "buddy-manager-adapter.h"

BuddyManagerAdapter::BuddyManagerAdapter(BuddyListModel *model) :
		QObject(model), Model(model)
{
	Model->setBuddyList(BuddyManager::instance()->items().toList());

	BuddyManager *manager = BuddyManager::instance();
	connect(manager, SIGNAL(buddyAdded(Buddy)),
			this, SLOT(buddyAdded(Buddy)), Qt::DirectConnection);
	connect(manager, SIGNAL(buddyRemoved(Buddy)),
			this, SLOT(buddyRemoved(Buddy)), Qt::DirectConnection);
}

BuddyManagerAdapter::~BuddyManagerAdapter()
{
	BuddyManager *manager = BuddyManager::instance();
	disconnect(manager, 0, this, 0);
}

void BuddyManagerAdapter::buddyAdded(const Buddy &buddy)
{
	Model->addBuddy(buddy);
}

void BuddyManagerAdapter::buddyRemoved(const Buddy &buddy)
{
	Model->removeBuddy(buddy);
}

#include "moc_buddy-manager-adapter.cpp"
