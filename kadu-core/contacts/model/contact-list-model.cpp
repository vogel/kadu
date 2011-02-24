/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

ContactListModel::ContactListModel(const QList<Contact> &list, QObject *parent) :
		QAbstractItemModel(parent), List(list)
{
}

ContactListModel::~ContactListModel()
{
}

QModelIndex ContactListModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

int ContactListModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 1;
}

int ContactListModel::rowCount(const QModelIndex &parentIndex) const
{
	return parentIndex.isValid() ? 0 : List.size();
}

QModelIndex ContactListModel::parent(const QModelIndex &child) const
{
	Q_UNUSED(child)

	return QModelIndex();
}

QVariant ContactListModel::data(const QModelIndex &index, int role) const
{
	return ContactDataExtractor::data(contactAt(index), role, true);
}

Contact ContactListModel::contactAt(const QModelIndex &index) const
{
    int row = index.row();
	if (row < 0 || row >= List.size())
		return Contact::null;

	return List.at(row);
}

QModelIndex ContactListModel::indexForValue(const QVariant &value) const
{
	int i = 0;
	foreach (const Contact &contact, List)
	{
		if (contact.ownerBuddy() == value.value<Buddy>())
			return createIndex(i, 0, 0);
		i++;
	}

	return QModelIndex();
}
