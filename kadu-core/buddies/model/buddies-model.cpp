/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

BuddiesModel::BuddiesModel(BuddyManager *manager, QObject *parent)
	: BuddiesModelBase(parent), Manager(manager)
{
	triggerAllAccountsRegistered();

	connect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	connect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
	connect(Manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));

	ContactManager *cm = ContactManager::instance();
	connect(cm, SIGNAL(contactAboutToBeAdded(Contact)),
			this, SLOT(contactAboutToBeAdded(Contact)));
	connect(cm, SIGNAL(contactAdded(Contact)),
			this, SLOT(contactAdded(Contact)));
	connect(cm, SIGNAL(contactAboutToBeRemoved(Contact)),
			this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(cm, SIGNAL(contactRemoved(Contact)),
			this, SLOT(contactRemoved(Contact)));
}

BuddiesModel::~BuddiesModel()
{
	triggerAllAccountsUnregistered();

	disconnect(Manager, SIGNAL(buddyAboutToBeAdded(Buddy &)),
			this, SLOT(buddyAboutToBeAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAdded(Buddy &)),
			this, SLOT(buddyAdded(Buddy &)));
	disconnect(Manager, SIGNAL(buddyAboutToBeRemoved(Buddy &)),
			this, SLOT(buddyAboutToBeRemoved(Buddy &)));
	disconnect(Manager, SIGNAL(buddyRemoved(Buddy &)),
			this, SLOT(buddyRemoved(Buddy &)));
}

int BuddiesModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return BuddiesModelBase::rowCount(parent);

	return Manager->count();
}

Buddy BuddiesModel::buddyAt(const QModelIndex &index) const
{
	QModelIndex parent = index.parent();
	return Manager->byIndex(parent.isValid() ? parent.row() : index.row());
}

const QModelIndex BuddiesModel::buddyIndex(Buddy buddy) const
{
	return index(Manager->indexOf(buddy), 0);
}

void BuddiesModel::buddyAboutToBeAdded(Buddy &buddy)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);

	connect(buddy, SIGNAL(updated()), this, SLOT(buddyUpdated()));
}

void BuddiesModel::buddyAdded(Buddy &buddy)
{
	endInsertRows();
}

void BuddiesModel::buddyAboutToBeRemoved(Buddy &buddy)
{
	int index = buddyIndex(buddy).row();
	beginRemoveRows(QModelIndex(), index, index);

	disconnect(buddy, SIGNAL(updated()), this, SLOT(buddyUpdated()));
}

void BuddiesModel::buddyRemoved(Buddy &buddy)
{
	endRemoveRows();
}

void BuddiesModel::buddyUpdated()
{
	Buddy buddy(sender());
	if (!buddy)
		return;

	QModelIndex index = buddyIndex(buddy);
	emit dataChanged(index, index);
}

void BuddiesModel::contactAboutToBeAdded(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex index = buddyIndex(buddy);
	if (!index.isValid())
		return;

	int count = rowCount(index);
	beginInsertRows(index, count, count);

	connect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
}

void BuddiesModel::contactAdded(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex index = buddyIndex(buddy);
	if (!index.isValid())
		return;

	endInsertRows();
}

void BuddiesModel::contactAboutToBeRemoved(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex index = buddyIndex(buddy);
	if (!index.isValid())
		return;

	int contactIndex = buddy.contacts().indexOf(contact);
	beginRemoveRows(index, contactIndex, contactIndex);

	disconnect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
}

void BuddiesModel::contactRemoved(Contact contact)
{
	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex index = buddyIndex(buddy);
	if (!index.isValid())
		return;

	endRemoveRows();
}

void BuddiesModel::contactUpdated()
{
	Contact contact(sender());
	if (!contact)
		return;

	Buddy buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	QModelIndex contactIndex = index(buddy.contacts().indexOf(contact), 0, buddyIndex(buddy));
	emit dataChanged(contactIndex, contactIndex);
}
