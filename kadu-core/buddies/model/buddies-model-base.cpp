/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "buddies/model/buddy-data-extractor.h"
#include "contacts/contact.h"
#include "contacts/model/contact-data-extractor.h"
#include "icons/icons-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddies-model-base.h"

BuddiesModelBase::BuddiesModelBase(QObject *parent) :
		QAbstractItemModel(parent), Checkable(false)
{
}

BuddiesModelBase::~BuddiesModelBase()
{
	triggerAllAccountsUnregistered();
}

void BuddiesModelBase::setCheckable(bool checkable)
{
	if (Checkable == checkable)
		return;

	beginResetModel();
	Checkable = checkable;
	endResetModel();
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

int BuddiesModelBase::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

QModelIndex BuddiesModelBase::index(int row, int column, const QModelIndex &parent) const
{
	if (row < 0 || column < 0)
		return QModelIndex();

	if (!parent.isValid()) // buddy
	{
		if (row >= rowCount())
			return QModelIndex(); // invalid

		const Buddy &buddy = buddyAt(row);
		Q_ASSERT(buddy.data());

		return createIndex(row, column, buddy.data());
	}

	// contact
	BuddyShared *parentBuddyShared = static_cast<BuddyShared *>(parent.internalPointer());
	Q_ASSERT(parentBuddyShared);

	Buddy parentBuddy(parentBuddyShared);
	const QList<Contact> &parentBuddyContacts = parentBuddy.contacts();
	if (row >= parentBuddyContacts.count())
		return QModelIndex();

	const Contact &contact = parentBuddyContacts.at(row);
	Q_ASSERT(contact.data());

	return createIndex(row, column, contact.data());
}

QModelIndex BuddiesModelBase::parent(const QModelIndex &child) const
{
	QObject *sharedData = static_cast<QObject *>(child.internalPointer());
	Q_ASSERT(sharedData);

	if (qobject_cast<BuddyShared *>(sharedData))
		return QModelIndex(); // buddies does not have parent

	ContactShared *childContactShared = qobject_cast<ContactShared *>(sharedData);
	Q_ASSERT(childContactShared);

	return index(buddyIndex(childContactShared->ownerBuddy()), 0);
}

int BuddiesModelBase::rowCount(const QModelIndex &parentIndex) const
{
	if (!parentIndex.isValid() || parent(parentIndex).isValid())
		return 0;

	const Buddy &buddy = parentIndex.data(BuddyRole).value<Buddy>();
	return buddy.contacts().count();
}

bool BuddiesModelBase::isCheckableIndex(const QModelIndex &index) const
{
	if (!Checkable)
		return false;

	if (BuddyRole != index.data(ItemTypeRole))
		return false;

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	return !buddy.isNull();
}

QFlags<Qt::ItemFlag> BuddiesModelBase::flags(const QModelIndex& index) const
{
	if (index.isValid())
	{
		if (isCheckableIndex(index))
			return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable;
		else
			return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
	}
	else
		return QAbstractItemModel::flags(index);
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

	QObject *sharedData = static_cast<QObject *>(index.internalPointer());
	Q_ASSERT(sharedData);

	BuddyShared *buddyShared = qobject_cast<BuddyShared *>(sharedData);
	if (buddyShared)
	{
		// use buddy role instead of what ContactDataExtractor would return
		if (ItemTypeRole == role)
			return BuddyRole;

		const Buddy &buddy = Buddy(buddyShared);

		if (Qt::CheckStateRole == role)
		{
			if (Checkable)
				return CheckedBuddies.contains(buddy) ? Qt::Checked : Qt::Unchecked;
			else
				return QVariant();
		}

		const Contact &contact = BuddyPreferredManager::instance()->preferredContact(buddy);

		return !contact.isNull()
				? ContactDataExtractor::data(contact, role, true)
				: BuddyDataExtractor::data(buddy, role);
	}

	ContactShared *contactShared = qobject_cast<ContactShared *>(sharedData);
	Q_ASSERT(contactShared);

	return ContactDataExtractor::data(Contact(contactShared), role, false);
}

bool BuddiesModelBase::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (Qt::CheckStateRole != role)
		return false;

	if (!Checkable)
		return false;

	if (index.parent().isValid())
		return false;

	if (BuddyRole != index.data(ItemTypeRole))
		return false;

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return false;

	Qt::CheckState checkState = static_cast<Qt::CheckState>(value.toInt());
	if (Qt::Checked == checkState)
	{
		CheckedBuddies.insert(buddy);
		emit checkedBuddiesChanged(CheckedBuddies);
		return true;
	}
	else if (Qt::Unchecked == checkState)
	{
		CheckedBuddies.remove(buddy);
		emit checkedBuddiesChanged(CheckedBuddies);
		return true;
	}

	return false;
}

BuddySet BuddiesModelBase::checkedBuddies() const
{
	return CheckedBuddies;
}

QModelIndexList BuddiesModelBase::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Buddy &buddy = value.value<Buddy>();
	if (buddy)
	{
		const int i = buddyIndex(buddy);
		if (-1 != i)
			result.append(index(i, 0));
		return result;
	}

	const Contact &contact = value.value<Contact>();
	if (!contact)
		return result;

	const Buddy &ownerBuddy = contact.ownerBuddy();
	const int contactIndexInBuddy = ownerBuddy.contacts().indexOf(contact);

	if (-1 != contactIndexInBuddy)
	{
		const int i = buddyIndex(buddy);
		if (-1 != i)
			result.append(index(i, 0).child(contactIndexInBuddy, 0));
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
