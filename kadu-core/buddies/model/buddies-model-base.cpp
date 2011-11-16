/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/model/buddy-data-extractor.h"
#include "contacts/contact.h"
#include "contacts/model/contact-data-extractor.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "icons/icons-manager.h"

#include "buddies-model-base.h"

BuddiesModelBase::BuddiesModelBase(QObject *parent) :
		QAbstractItemModel(parent)
{
}

BuddiesModelBase::~BuddiesModelBase()
{
	triggerAllAccountsUnregistered();
}

void BuddiesModelBase::accountRegistered(Account account)
{
	connect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesModelBase::accountUnregistered(Account account)
{
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesModelBase::buddyStatusChanged(Contact contact, Status oldStatus)
{
	Q_UNUSED(oldStatus)

	const QModelIndexList& indexes = indexListForValue(contact.ownerBuddy());
	foreach (const QModelIndex &index, indexes)
		if (index.isValid())
			emit dataChanged(index, index);
}

QModelIndex BuddiesModelBase::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

int BuddiesModelBase::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int BuddiesModelBase::rowCount(const QModelIndex &parentIndex) const
{
	if (!parentIndex.isValid() || parent(parentIndex).isValid())
		return 0;

	const Buddy &buddy = parentIndex.data(BuddyRole).value<Buddy>();
	return buddy.contacts().count();
}

QFlags<Qt::ItemFlag> BuddiesModelBase::flags(const QModelIndex& index) const
{
	if (index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	else
		return QAbstractItemModel::flags(index);
}

QModelIndex BuddiesModelBase::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();
	else
		return index(child.internalId(), 0, QModelIndex());
}

Contact BuddiesModelBase::buddyContact(const QModelIndex &index, int accountIndex) const
{
	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return Contact::null;

	QList<Contact> contacts = buddy.contacts();
	if (accountIndex < 0 || contacts.size() <= accountIndex)
		return Contact::null;

	return contacts.at(accountIndex);
}

QVariant BuddiesModelBase::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	QModelIndex parentIndex = parent(index);
	if (!parentIndex.isValid())
	{
		if (ItemTypeRole == role)
			return BuddyRole;

		const Buddy &buddy = buddyAt(index.row());
		const Contact &contact = BuddyPreferredManager::instance()->preferredContact(buddy);

		return !contact.isNull()
				? ContactDataExtractor::data(contact, role, true)
				: BuddyDataExtractor::data(buddy, role);
	}
	else
		return ContactDataExtractor::data(buddyContact(parentIndex, index.row()), role, false);
}

QModelIndexList BuddiesModelBase::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Buddy &buddy = value.value<Buddy>();
	if (buddy)
	{
		result.append(index(buddyIndex(buddy), 0));
		return result;
	}

	const Contact &contact = value.value<Contact>();
	if (contact)
	{
		const Buddy &ownerBuddy = contact.ownerBuddy();
		const int contactIndexInBuddy = ownerBuddy.contacts().indexOf(contact);
		result.append(index(buddyIndex(ownerBuddy), 0).child(contactIndexInBuddy, 0));
		return result;
	}


	return result;
}

// D&D

QStringList BuddiesModelBase::mimeTypes() const
{
	return BuddyListMimeDataHelper::mimeTypes();
}

QMimeData * BuddiesModelBase::mimeData(const QModelIndexList &indexes) const
{
	BuddyList list;
	foreach (const QModelIndex &index, indexes)
	{
		Buddy con = index.data(BuddyRole).value<Buddy>();
		if (con.isNull())
			continue;
		list << con;
	}

	return BuddyListMimeDataHelper::toMimeData(list);
}
