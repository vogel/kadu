 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-contact-model.h"

BuddyContactModel::BuddyContactModel(Buddy buddy, QObject *parent) :
		QAbstractListModel(parent), SourceBuddy(buddy)
{
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact *)),
			this, SLOT(contactAboutToBeAdded(Contact *)));
	connect(ContactManager::instance(), SIGNAL(contactAdded(Contact *)),
			this, SLOT(contactAdded(Contact *)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact *)),
			this, SLOT(contactAboutToBeRemoved(Contact *)));
	connect(ContactManager::instance(), SIGNAL(contactRemoved(Contact *)),
			this, SLOT(contactRemoved(Contact *)));
}

BuddyContactModel::~BuddyContactModel()
{
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeAdded(Contact *)),
			this, SLOT(contactAboutToBeAdded(Contact *)));
	disconnect(ContactManager::instance(), SIGNAL(contactAdded(Contact *)),
			this, SLOT(contactAdded(Contact *)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeRemoved(Contact *)),
			this, SLOT(contactAboutToBeRemoved(Contact *)));
	disconnect(ContactManager::instance(), SIGNAL(contactRemoved(Contact *)),
			this, SLOT(contactRemoved(Contact *)));
}

int BuddyContactModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

int BuddyContactModel::rowCount(const QModelIndex &parent) const
{
	return SourceBuddy.accountDatas().count();
}

QVariant BuddyContactModel::data(const QModelIndex &index, int role) const
{
	Contact *data = contact(index);
	if (0 == data)
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			if (index.column() == 0) // long or shor name?
				return data->id();
			else
				return QString("%1 (%2)").arg(data->id(), data->account().name());

		case Qt::DecorationRole:
			return data->account().protocolHandler()->icon();

		case AccountDataRole:
			return QVariant::fromValue<Contact *>(data);

		default:
			return QVariant();
	}
}

QVariant BuddyContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

Contact * BuddyContactModel::contact(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	if (index.row() < 0 || index.row() >= rowCount())
		return 0;

	return SourceBuddy.accountDatas().at(index.row());
}

int BuddyContactModel::contactIndex(Contact *data)
{
	return SourceBuddy.accountDatas().indexOf(data);
}

QModelIndex BuddyContactModel::contactModelIndex(Contact *data)
{
	return createIndex(contactIndex(data), 0, 0);
}

void BuddyContactModel::contactAboutToBeAdded(Contact *data)
{
	int count = rowCount();
	beginInsertRows(QModelIndex(), count, count);
}

void BuddyContactModel::contactAdded(Contact *data)
{
	endInsertRows();
}

void BuddyContactModel::contactAboutToBeRemoved(Contact *data)
{
	int index = contactIndex(data);
	beginRemoveRows(QModelIndex(), index, index);
}

void BuddyContactModel::contactRemoved(Contact *data)
{
	endRemoveRows();
}
