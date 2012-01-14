/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "buddies/buddy.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"

#include "model/history-type.h"
#include "history-chats-model.h"
#include "history-tree-item.h"

HistoryChatsModel::HistoryChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
}

HistoryChatsModel::~HistoryChatsModel()
{
}

int HistoryChatsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 1;
}

int HistoryChatsModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 2;

	if (parent.parent().isValid())
		return 0;

	if (parent.row() < 0)
		return 0;

	switch (parent.row())
	{
		case 0:
			return StatusBuddies.size();
		case 1:
			return SmsRecipients.size();
	}

	return 0;
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	return hasIndex(row, column, parent) ? createIndex(row, column, parent.isValid() ? parent.row() : -1) : QModelIndex();
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();

	return createIndex(child.internalId(), 0, -1);
}

QVariant HistoryChatsModel::statusData(const QModelIndex &index, int role) const
{
	if (!index.parent().isValid())
	{
		switch (role)
		{
			case Qt::DisplayRole:
				return tr("Statuses");
			case Qt::DecorationRole:
				return KaduIcon("protocols/common/online").icon();
		}
		return QVariant();
	}

	if (index.row() < 0 || index.row() >= StatusBuddies.size())
		return QVariant();

	Buddy buddy = StatusBuddies.at(index.row());
	switch (role)
	{
		case Qt::DisplayRole:
			return buddy.display();
		case BuddyRole:
			return QVariant::fromValue<Buddy>(buddy);
		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(buddy));
	}
	return QVariant();
}

QVariant HistoryChatsModel::smsRecipientData(const QModelIndex &index, int role) const
{
	if (!index.parent().isValid())
	{
		switch (role)
		{
			case Qt::DisplayRole:
				return tr("SMSes");
			case Qt::DecorationRole:
				return KaduIcon("phone").icon();
		}
		return QVariant();
	}

	if (index.row() < 0 || index.row() >= SmsRecipients.size())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return SmsRecipients.at(index.row());
		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(SmsRecipients.at(index.row())));
	}
	return QVariant();
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().parent().isValid())
		return QVariant();

	qint64 row = index.parent().isValid() ? index.internalId() : index.row();
	if (row < 0)
		return QVariant();

	switch (row)
	{
		case 0: return statusData(index, role);
		case 1: return smsRecipientData(index, role);
	}

	return QVariant();
}

void HistoryChatsModel::clearStatusBuddies()
{
	if (!StatusBuddies.isEmpty())
	{
		beginResetModel();
		StatusBuddies.clear();
		endResetModel();
	}
}

void HistoryChatsModel::clearSmsRecipients()
{
	if (!SmsRecipients.isEmpty())
	{
		beginResetModel();
		SmsRecipients.clear();
		endResetModel();
	}
}

void HistoryChatsModel::setStatusBuddies(const QVector<Buddy> &buddies)
{
	clearStatusBuddies();

	if (!buddies.isEmpty())
	{
		beginResetModel();
		StatusBuddies = buddies;
		endResetModel();
	}
}

void HistoryChatsModel::setSmsRecipients(const QList<QString> &smsRecipients)
{
	clearSmsRecipients();

	if (!smsRecipients.isEmpty())
	{
		beginResetModel();
		SmsRecipients = smsRecipients;
		endResetModel();
	}
}

QModelIndex HistoryChatsModel::statusIndex() const
{
	return index(0, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::statusBuddyIndex(const Buddy &buddy) const
{
	QModelIndex parent = statusIndex();
	if (!parent.isValid())
		return QModelIndex();

	int row = StatusBuddies.indexOf(buddy);
	return index(row, 0, parent);
}

QModelIndex HistoryChatsModel::smsIndex() const
{
	return index(1, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::smsRecipientIndex(const QString &recipient) const
{
	QModelIndex parent = smsIndex();
	if (!parent.isValid())
		return QModelIndex();

	int row = SmsRecipients.indexOf(recipient);
	return index(row, 0, parent);
}
