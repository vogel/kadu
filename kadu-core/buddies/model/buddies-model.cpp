/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"

#include "protocols/protocol.h"

#include "buddies-model.h"

BuddiesModel::BuddiesModel(QObject *parent) :
		BuddiesModelBase(parent)
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
			this, SLOT(contactAttached(Contact, bool)));
	connect(cm, SIGNAL(contactAboutToBeDetached(Contact, bool)),
			this, SLOT(contactAboutToBeDetached(Contact, bool)));
	connect(cm, SIGNAL(contactDetached(Contact, Buddy)),
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
			this, SLOT(contactAttached(Contact, bool)));
	disconnect(cm, SIGNAL(contactAboutToBeDetached(Contact, bool)),
			this, SLOT(contactAboutToBeDetached(Contact, bool)));
	disconnect(cm, SIGNAL(contactDetached(Contact, Buddy)),
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

Buddy BuddiesModel::buddyAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	int row = parent.isValid() ? parent.row() : index.row();

	if (IncludeMyself && (row == BuddyManager::instance()->count()))
		return Core::instance()->myself();
	else
		return BuddyManager::instance()->byIndex(row);
}

QModelIndex BuddiesModel::indexForValue(const QVariant &value) const
{
	Buddy buddy = value.value<Buddy>();
	int result;
	if (IncludeMyself && buddy == Core::instance()->myself())
		result = BuddyManager::instance()->count();
	else
		result = BuddyManager::instance()->indexOf(buddy);

	return index(result, 0);
}

void BuddiesModel::buddyAboutToBeAdded(Buddy &buddy)
{
	Q_UNUSED(buddy)

	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void BuddiesModel::buddyAdded(Buddy &buddy)
{
	Q_UNUSED(buddy)

	endInsertRows();

	// force refresh on proxy sorring model
	// setDynamicSortFilter does not work properly when adding/removing items, only when changing item data
	// this is Qt bug
	// see: http://kadu.net/mantis/view.php?id=2167
	QModelIndex index = indexForValue(buddy);
	emit dataChanged(index, index);
}

void BuddiesModel::buddyAboutToBeRemoved(Buddy &buddy)
{
	int index = indexForValue(buddy).row();
	beginRemoveRows(QModelIndex(), index, index);
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
	QModelIndex index = indexForValue(buddy);
	if (index.isValid())
		emit dataChanged(index, index);
}

void BuddiesModel::contactAboutToBeAttached(Contact contact, Buddy nearFutureBuddy)
{
	Q_UNUSED(contact)

	QModelIndex index = indexForValue(nearFutureBuddy);
	if (!index.isValid())
		return;

	int count = nearFutureBuddy.contacts().length();
	beginInsertRows(index, count, count);
}

void BuddiesModel::contactAttached(Contact contact, bool reattached)
{
	Q_UNUSED(reattached)

	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
	if (!index.isValid())
		return;

	endInsertRows();
}

void BuddiesModel::contactAboutToBeDetached(Contact contact, bool reattached)
{
	Q_UNUSED(reattached)

	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
	if (!index.isValid())
		return;

	int contactIndex = buddy.contacts().indexOf(contact);
	beginRemoveRows(index, contactIndex, contactIndex);
}

void BuddiesModel::contactDetached(Contact contact, Buddy previousBuddy)
{
	Q_UNUSED(contact)

	QModelIndex index = indexForValue(previousBuddy);
	if (!index.isValid())
		return;

	endRemoveRows();
}

void BuddiesModel::contactUpdated(Contact &contact)
{
	if (!contact)
		return;

	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex indexOfBuddy = indexForValue(buddy);
	if (!indexOfBuddy.isValid())
		return;

	QModelIndex contactIndex = index(buddy.contacts().indexOf(contact), 0, indexOfBuddy);

	emit dataChanged(indexOfBuddy, indexOfBuddy);
	emit dataChanged(contactIndex, contactIndex);
}

void BuddiesModel::setIncludeMyself(bool includeMyself)
{
	if (IncludeMyself == includeMyself)
		return;

	if (IncludeMyself)
		beginRemoveRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
	else
		beginInsertRows(QModelIndex(), rowCount(), rowCount());

	IncludeMyself = includeMyself;

	if (!IncludeMyself)
		endRemoveRows();
	else
		endInsertRows();
}
