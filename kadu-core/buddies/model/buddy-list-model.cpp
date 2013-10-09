/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "contacts/model/contact-data-extractor.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "talkable/talkable.h"

#include "buddy-data-extractor.h"
#include "buddy-list-model.h"

BuddyListModel::BuddyListModel(QObject *parent) :
		QAbstractItemModel(parent), Checkable(false)
{
	QHash<int, QByteArray> roles;
	roles[Qt::DisplayRole] = "display";
	roles[AvatarPathRole] = "avatar";
	roles[StatusIconPath] = "statusIcon";
	setRoleNames(roles);

	triggerAllAccountsRegistered();

	connect(ContactManager::instance(), SIGNAL(contactUpdated(Contact)),
	        this, SLOT(contactUpdated(Contact)), Qt::DirectConnection);
}

BuddyListModel::~BuddyListModel()
{
	setBuddyList(BuddyList());

	triggerAllAccountsUnregistered();

	disconnect(ContactManager::instance(), 0, this, 0);
}

Buddy BuddyListModel::buddyFromVariant(const QVariant &variant) const
{
	Buddy buddy = variant.value<Buddy>();
	if (buddy)
		return buddy;
	Talkable talkable = variant.value<Talkable>();
	if (talkable.isValidBuddy())
		return talkable.toBuddy();
	else
		return Buddy::null;
}

Contact BuddyListModel::contactFromVariant(const QVariant &variant) const
{
	Contact contact = variant.value<Contact>();
	if (contact)
		return contact;
	Talkable talkable = variant.value<Talkable>();
	if (talkable.isValidContact())
		return talkable.toContact();
	else
		return Contact::null;
}

bool BuddyListModel::isCheckableIndex(const QModelIndex &index) const
{
	if (!Checkable)
		return false;

	if (BuddyRole != index.data(ItemTypeRole))
		return false;

	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	return !buddy.isNull();
}

Contact BuddyListModel::buddyContact(const QModelIndex &index, int contactIndex) const
{
	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();
	if (!buddy)
		return Contact::null;

	QList<Contact> contacts = buddy.contacts();
	if (contactIndex < 0 || contacts.size() <= contactIndex)
		return Contact::null;

	return contacts.at(contactIndex);
}

void BuddyListModel::connectBuddy(const Buddy &buddy)
{
	connect(buddy, SIGNAL(updated()), this, SLOT(buddyUpdated()));
	connect(buddy, SIGNAL(contactAboutToBeRemoved(Contact)),
	        this, SLOT(contactAboutToBeRemoved(Contact)));
	connect(buddy, SIGNAL(contactRemoved(Contact)),
	        this, SLOT(contactRemoved(Contact)));
	connect(buddy, SIGNAL(contactAboutToBeAdded(Contact)),
	        this, SLOT(contactAboutToBeAdded(Contact)));
	connect(buddy, SIGNAL(contactAdded(Contact)),
	        this, SLOT(contactAdded(Contact)));
}

void BuddyListModel::disconnectBuddy(const Buddy &buddy)
{
	disconnect(buddy, 0, this, 0);
}

void BuddyListModel::buddyUpdated()
{
	Buddy buddy(sender());
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	emit dataChanged(index, index);
}

void BuddyListModel::contactUpdated(const Contact &contact)
{
	const Buddy &buddy = contact.ownerBuddy();
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &buddyIndex = indexes.at(0);
	if (!buddyIndex.isValid())
		return;

	const QModelIndex &contactIndex = index(buddy.contacts().indexOf(contact), 0, buddyIndex);

	emit dataChanged(buddyIndex, buddyIndex);
	emit dataChanged(contactIndex, contactIndex);
}

void BuddyListModel::contactStatusChanged(const Contact &contact, const Status &oldStatus)
{
	Q_UNUSED(oldStatus)

	const QModelIndexList& indexes = indexListForValue(contact.ownerBuddy());
	foreach (const QModelIndex &index, indexes)
		if (index.isValid())
			emit dataChanged(index, index);
}

void BuddyListModel::contactAboutToBeAdded(const Contact &contact)
{
	Q_UNUSED(contact)

	Buddy buddy(sender());
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	int count = buddy.contacts().size();
	beginInsertRows(index, count, count);
}

void BuddyListModel::contactAdded(const Contact &contact)
{
	Q_UNUSED(contact)

	Buddy buddy(sender());
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	endInsertRows();
}

void BuddyListModel::contactAboutToBeRemoved(const Contact &contact)
{
	Buddy buddy(sender());
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	int contactIndex = buddy.contacts().indexOf(contact);
	beginRemoveRows(index, contactIndex, contactIndex);
}

void BuddyListModel::contactRemoved(const Contact &contact)
{
	Q_UNUSED(contact)

	Buddy buddy(sender());
	if (!buddy)
		return;

	const QModelIndexList &indexes = indexListForValue(buddy);
	if (indexes.isEmpty())
		return;

	Q_ASSERT(indexes.size() == 1);

	const QModelIndex &index = indexes.at(0);
	if (!index.isValid())
		return;

	endRemoveRows();
}

void BuddyListModel::setBuddyList(const BuddyList &list)
{
	beginResetModel();

	foreach (const Buddy &buddy, List)
		disconnectBuddy(buddy);

	List = list;

	foreach (const Buddy &buddy, List)
		connectBuddy(buddy);

	endResetModel();
}

void BuddyListModel::addBuddy(const Buddy &buddy)
{
	if (List.contains(buddy))
		return;

	connectBuddy(buddy);

	beginInsertRows(QModelIndex(), List.count(), List.count());
	List.append(buddy);
	endInsertRows();

	dataChanged(index(List.count() - 1, 0), index(List.count() - 1, 0));
}

void BuddyListModel::removeBuddy(const Buddy &buddy)
{
	int index = List.indexOf(buddy);
	if (-1 == index)
		return;

	disconnectBuddy(buddy);

	beginRemoveRows(QModelIndex(), index, index);
	List.removeAt(index);
	endRemoveRows();
}

void BuddyListModel::setCheckable(bool checkable)
{
	if (Checkable == checkable)
		return;

	beginResetModel();
	Checkable = checkable;
	endResetModel();

	emit checkedBuddiesChanged(checkedBuddies());
}

QModelIndex BuddyListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (row < 0 || column < 0)
		return QModelIndex();

	if (!parent.isValid()) // buddy
	{
		if (row >= List.count())
			return QModelIndex(); // invalid

		const Buddy &buddy = List.at(row);
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

QModelIndex BuddyListModel::parent(const QModelIndex &child) const
{
	QObject *sharedData = static_cast<QObject *>(child.internalPointer());
	Q_ASSERT(sharedData);

	if (qobject_cast<BuddyShared *>(sharedData))
		return QModelIndex(); // buddies does not have parent

	ContactShared *childContactShared = qobject_cast<ContactShared *>(sharedData);
	Q_ASSERT(childContactShared);

	return index(List.indexOf(childContactShared->ownerBuddy()), 0);
}

int BuddyListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int BuddyListModel::rowCount(const QModelIndex &parentIndex) const
{
	if (!parentIndex.isValid())
		return List.count();
	if (parentIndex.parent().isValid())
		return 0;

	const Buddy &buddy = parentIndex.data(BuddyRole).value<Buddy>();
	return buddy.contacts().count();
}

QFlags<Qt::ItemFlag> BuddyListModel::flags(const QModelIndex& index) const
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

QVariant BuddyListModel::data(const QModelIndex &index, int role) const
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

		return TalkableRole != role && !contact.isNull()
				? ContactDataExtractor::data(contact, role, true)
				: BuddyDataExtractor::data(buddy, role);
	}

	ContactShared *contactShared = qobject_cast<ContactShared *>(sharedData);
	Q_ASSERT(contactShared);

	return ContactDataExtractor::data(Contact(contactShared), role, false);
}

bool BuddyListModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

BuddySet BuddyListModel::checkedBuddies() const
{
	if (Checkable)
		return CheckedBuddies;
	else
		return BuddySet();
}

QModelIndexList BuddyListModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Buddy &buddy = buddyFromVariant(value);
	if (buddy)
	{
		const int i = List.indexOf(buddy);
		if (-1 != i)
			result.append(index(i, 0));
		return result;
	}

	const Contact &contact = contactFromVariant(value);
	if (!contact)
		return result;

	const Buddy &ownerBuddy = contact.ownerBuddy();
	const int contactIndexInBuddy = ownerBuddy.contacts().indexOf(contact);

	if (-1 != contactIndexInBuddy)
	{
		const int i = List.indexOf(buddy);
		if (-1 != i)
			result.append(index(i, 0).child(contactIndexInBuddy, 0));
	}

	return result;
}

// D&D

QStringList BuddyListModel::mimeTypes() const
{
	return BuddyListMimeDataHelper::mimeTypes();
}

QMimeData * BuddyListModel::mimeData(const QModelIndexList &indexes) const
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

void BuddyListModel::accountRegistered(Account account)
{
	connect(account, SIGNAL(buddyStatusChanged(Contact,Status)), this, SLOT(contactStatusChanged(Contact,Status)));
}

void BuddyListModel::accountUnregistered (Account account)
{
	disconnect(account, 0, this, 0);
}

#include "moc_buddy-list-model.cpp"
