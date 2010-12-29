/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "icons-manager.h"

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

	QModelIndex index = indexForValue(contact.ownerBuddy());

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

	Buddy buddy = buddyAt(parentIndex);
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

QVariant BuddiesModelBase::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Contact BuddiesModelBase::buddyDefaultContact(const QModelIndex &index) const
{
	Buddy buddy = buddyAt(index);
	if (buddy.isNull())
		return Contact::null;

	return BuddyPreferredManager::instance()->preferredContact(buddy);
}

Contact BuddiesModelBase::buddyContact(const QModelIndex &index, int accountIndex) const
{
	Buddy buddy = buddyAt(index);
	if (buddy.isNull())
		return Contact::null;

	QList<Contact> contacts = buddy.contacts();
	if (contacts.size() <= accountIndex)
		return Contact::null;

	return contacts[accountIndex];
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

		Contact contact = buddyDefaultContact(index);
		return !contact.isNull()
				? ContactDataExtractor::data(contact, role, true)
				: BuddyDataExtractor::data(buddyAt(index), role);
	}
	else
		return ContactDataExtractor::data(buddyContact(parentIndex, index.row()), role, false);
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
