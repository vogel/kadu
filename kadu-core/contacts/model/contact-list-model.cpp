/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/model/contact-data-extractor.h"

#include "contact-list-model.h"

ContactListModel::ContactListModel(QObject *parent) :
		QAbstractItemModel(parent)
{
}

ContactListModel::~ContactListModel()
{
}

void ContactListModel::connectContact(const Contact &contact)
{
	connect(contact, SIGNAL(updated()), this, SLOT(contactUpdated()));
}

void ContactListModel::disconnectContact(const Contact &contact)
{
	disconnect(contact, 0, this, 0);
}

void ContactListModel::setContactList(const QVector<Contact> &contacts)
{
	beginResetModel();

	foreach (const Contact &contact, List)
		disconnectContact(contact);
	List = contacts;
	foreach (const Contact &contact, List)
		connectContact(contact);

	endResetModel();
}

void ContactListModel::addContact(const Contact &contact)
{
	if (List.contains(contact))
		return;

	connectContact(contact);

	beginInsertRows(QModelIndex(), List.count(), List.count());
	List.append(contact);
	endInsertRows();
}

void ContactListModel::removeContact(const Contact &contact)
{
	int index = List.indexOf(contact);
	if (-1 == index)
		return;

	disconnectContact(contact);

	beginRemoveRows(QModelIndex(), index, index);
	List.remove(index);
	endRemoveRows();
}

void ContactListModel::contactUpdated()
{
	ContactShared *contactShared = qobject_cast<ContactShared *>(sender());
	if (!contactShared)
		return;

	int row = List.indexOf(Contact(contactShared));
	if (row < 0)
		return;

	const QModelIndex &contactIndex = index(row, 0);
	emit dataChanged(contactIndex, contactIndex);
}

QModelIndex ContactListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() || !hasIndex(row, column, parent))
		return QModelIndex();

	return createIndex(row, column, List.at(row).data());
}

QModelIndex ContactListModel::parent(const QModelIndex &child) const
{
	Q_UNUSED(child)

	return QModelIndex();
}

int ContactListModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int ContactListModel::rowCount(const QModelIndex &parentIndex) const
{
	return parentIndex.isValid() ? 0 : List.size();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
	QObject *sharedData = static_cast<QObject *>(index.internalPointer());
	Q_ASSERT(sharedData);

	ContactShared *contact = qobject_cast<ContactShared *>(sharedData);
	if (!contact)
		return QVariant();

	return ContactDataExtractor::data(Contact(contact), role, true);
}

QModelIndexList ContactListModel::indexListForValue(const QVariant &value) const
{
	QModelIndexList result;

	const Buddy &buddy = value.value<Buddy>();

	const int size = List.size();
	for (int i = 0; i < size; i++)
	{
		const Contact &contact = List.at(i);
		if (contact.ownerBuddy() == buddy)
			result.append(index(i, 0));
	}

	return result;
}

#include "moc_contact-list-model.cpp"
