/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"

#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"

#include "protocols/protocol.h"

#include "buddies-model.h"

BuddiesModel::BuddiesModel(QObject *parent) :
		BuddiesModelBase(parent), DetachingOrAttaching(false)
{
	IncludeMyself = config_file.readBoolEntry("General", "ShowMyself", false);

	triggerAllAccountsRegistered();

	BuddyManager *manager = BuddyManager::instance();
	connect(manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	connect(manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	connect(manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
	connect(manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));
	connect(manager, SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));

	connect(Core::instance()->myself(), SIGNAL(updated()),
			this, SLOT(myselfBuddyUpdated()));

	ContactManager *cm = ContactManager::instance();
	connect(cm, SIGNAL(contactAboutToBeAttached(Contact, Buddy)),
			this, SLOT(contactAboutToBeAttached(Contact, Buddy)));
	connect(cm, SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact)));
	connect(cm, SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	connect(cm, SIGNAL(contactDetached(Contact, Buddy, bool)),
			this, SLOT(contactDetached(Contact, Buddy)));
	connect(cm, SIGNAL(contactUpdated(Contact&)),
			   this, SLOT(contactUpdated(Contact&)));
}

BuddiesModel::~BuddiesModel()
{
	triggerAllAccountsUnregistered();

	BuddyManager *manager = BuddyManager::instance();
	disconnect(manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	disconnect(manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	disconnect(manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
	disconnect(manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));
	disconnect(manager, SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &)));

	ContactManager *cm = ContactManager::instance();
	disconnect(cm, SIGNAL(contactAboutToBeAttached(Contact, Buddy)),
			this, SLOT(contactAboutToBeAttached(Contact, Buddy)));
	disconnect(cm, SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact)));
	disconnect(cm, SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	disconnect(cm, SIGNAL(contactDetached(Contact, Buddy, bool)),
			this, SLOT(contactDetached(Contact, Buddy)));
	disconnect(cm, SIGNAL(contactUpdated(Contact&)),
			   this, SLOT(contactUpdated(Contact&)));
}

int BuddiesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return BuddiesModelBase::rowCount(parent);

	return BuddyManager::instance()->count() + (IncludeMyself ? 1 : 0);
}

int BuddiesModel::buddyIndex(const Buddy &buddy) const
{
	if (IncludeMyself && buddy == Core::instance()->myself())
		return BuddyManager::instance()->count();
	else
		return BuddyManager::instance()->indexOf(buddy);
}

Buddy BuddiesModel::buddyAt(int index) const
{
	if (IncludeMyself && (index == BuddyManager::instance()->count()))
		return Core::instance()->myself();
	else
		return BuddyManager::instance()->byIndex(index);
}

void BuddiesModel::buddyAboutToBeAdded(Buddy &buddy)
{
	Q_UNUSED(buddy)

	int count = rowCount() - (IncludeMyself ? 1 : 0);
	beginInsertRows(QModelIndex(), count, count);
}

void BuddiesModel::buddyAdded(Buddy &buddy)
{
	endInsertRows();

	// force refresh on proxy sorring model
	// setDynamicSortFilter does not work properly when adding/removing items, only when changing item data
	// this is Qt bug
	// see bug #2167

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	emit dataChanged(index, index);
}

void BuddiesModel::buddyAboutToBeRemoved(Buddy &buddy)
{
	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	beginRemoveRows(QModelIndex(), index.row(), index.row());
}

void BuddiesModel::buddyRemoved(Buddy &buddy)
{
	Q_UNUSED(buddy)

	endRemoveRows();
}

void BuddiesModel::myselfBuddyUpdated()
{
	if (IncludeMyself)
	{
		Buddy myself = Core::instance()->myself();
		buddyUpdated(myself);
	}
}

void BuddiesModel::buddyUpdated(Buddy &buddy)
{
	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	emit dataChanged(index, index);
}

void BuddiesModel::contactAboutToBeAttached(Contact contact, Buddy nearFutureBuddy)
{
	Q_UNUSED(contact)

	const QModelIndexList &indexes = indexListForValue(nearFutureBuddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	DetachingOrAttaching = true;

	int count = nearFutureBuddy.contacts().size();
	beginInsertRows(index, count, count);
}

void BuddiesModel::contactAttached(Contact contact)
{
	DetachingOrAttaching = false;

	const Buddy &buddy = contact.ownerBuddy();

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	endInsertRows();
}

void BuddiesModel::contactAboutToBeDetached(Contact contact)
{
	const Buddy &buddy = contact.ownerBuddy();

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	DetachingOrAttaching = true;

	int contactIndex = buddy.contacts().indexOf(contact);
	beginRemoveRows(index, contactIndex, contactIndex);
}

void BuddiesModel::contactDetached(Contact contact, Buddy previousBuddy)
{
	Q_UNUSED(contact)

	DetachingOrAttaching = false;

	const QModelIndexList &indexes = indexListForValue(previousBuddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	endRemoveRows();
}

void BuddiesModel::contactUpdated(Contact &contact)
{
	if (DetachingOrAttaching)
		return;

	const Buddy &buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &indexOfBuddy = indexes.at(0);
	if (!indexOfBuddy.isValid())
		return;

	const QModelIndex &contactIndex = index(buddy.contacts().indexOf(contact), 0, indexOfBuddy);

	emit dataChanged(indexOfBuddy, indexOfBuddy);
	emit dataChanged(contactIndex, contactIndex);
}

void BuddiesModel::setIncludeMyself(bool includeMyself)
{
	if (IncludeMyself == includeMyself)
		return;

	int count = BuddyManager::instance()->count();
	if (IncludeMyself)
		beginRemoveRows(QModelIndex(), count, count);
	else
		beginInsertRows(QModelIndex(), count, count);

	IncludeMyself = includeMyself;

	if (!IncludeMyself)
		endRemoveRows();
	else
		endInsertRows();
}
