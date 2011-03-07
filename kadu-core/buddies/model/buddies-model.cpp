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
#include "contacts/contact-manager.h"
#include "contacts/contact.h"

#include "protocols/protocol.h"

#include "buddies-model.h"

BuddiesModel::BuddiesModel(QObject *parent) :
		BuddiesModelBase(parent)
{
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

	ContactManager *cm = ContactManager::instance();
	connect(cm, SIGNAL(contactAboutToBeAttached(Contact)),
			this, SLOT(contactAboutToBeAttached(Contact)));
	connect(cm, SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(cm, SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	connect(cm, SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
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
	disconnect(cm, SIGNAL(contactAboutToBeAttached(Contact)),
			this, SLOT(contactAboutToBeAttached(Contact)));
	disconnect(cm, SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(cm, SIGNAL(contactAboutToBeDetached(Contact)),
			this, SLOT(contactAboutToBeDetached(Contact)));
	disconnect(cm, SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	disconnect(cm, SIGNAL(contactUpdated(Contact&)),
			   this, SLOT(contactUpdated(Contact&)));
}

int BuddiesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return BuddiesModelBase::rowCount(parent);

	return BuddyManager::instance()->count();
}

Buddy BuddiesModel::buddyAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return BuddyManager::instance()->byIndex(parent.isValid() ? parent.row() : index.row());
}

QModelIndex BuddiesModel::indexForValue(const QVariant &value) const
{
	Buddy buddy = value.value<Buddy>();
	int result = BuddyManager::instance()->indexOf(buddy);
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

void BuddiesModel::buddyUpdated(Buddy &buddy)
{
	QModelIndex index = indexForValue(buddy);
	if (index.isValid())
		emit dataChanged(index, index);
}

void BuddiesModel::contactAboutToBeAttached(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
	if (!index.isValid())
		return;

	int count = rowCount(index);
	beginInsertRows(index, count, count);
}

void BuddiesModel::contactAttached(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
	if (!index.isValid())
		return;

	endInsertRows();
}

void BuddiesModel::contactAboutToBeDetached(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
	if (!index.isValid())
		return;

	int contactIndex = buddy.contacts().indexOf(contact);
	beginRemoveRows(index, contactIndex, contactIndex);
}

void BuddiesModel::contactDetached(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();

	QModelIndex index = indexForValue(buddy);
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
