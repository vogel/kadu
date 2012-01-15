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
	if (parent.isValid())
		return 0;

	return SmsRecipients.size();
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid())
		return QModelIndex();
	return createIndex(row, column, 0);
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	Q_UNUSED(child);

	return QModelIndex();
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= SmsRecipients.size())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
			return SmsRecipients.at(index.row());
		case Qt::DecorationRole:
			return KaduIcon("phone").icon();
		case HistoryItemRole:
			return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(SmsRecipients.at(index.row())));
	}

	return QVariant();
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

QModelIndex HistoryChatsModel::smsRecipientIndex(const QString &recipient) const
{
	return index(SmsRecipients.indexOf(recipient), 0, QModelIndex());
}
