/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QTextDocument>

#include "chat/chat.h"
#include "model/roles.h"

#include "history-tree-item.h"
#include "history.h"

#include "sms-dates-model.h"

SmsDatesModel::SmsDatesModel(const QString &recipient, const QList<QDate> &dates, QObject *parent) :
		QAbstractListModel(parent), Recipient(recipient), Dates(dates)
{
	Cache = new QMap<QDate, ItemCachedData>();
}

SmsDatesModel::~SmsDatesModel()
{
	delete Cache;
	Cache = 0;
}

int SmsDatesModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

int SmsDatesModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Dates.size();
}

QVariant SmsDatesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation != Qt::Horizontal)
		return QVariant();

	switch (section)
	{
		case 0: return tr("Date");
		case 1: return tr("Length");
	}

	return QVariant();
}

int SmsDatesModel::fetchSize(const QDate &date) const
{
	return History::instance()->smsCount(Recipient, date);
}

SmsDatesModel::ItemCachedData SmsDatesModel::fetchCachedData(const QDate &date) const
{
	if (Cache->contains(date))
		return Cache->value(date);

	ItemCachedData cache;
	cache.size = fetchSize(date);
	Cache->insert(date, cache);

	return cache;
}

QVariant SmsDatesModel::data(const QModelIndex &index, int role) const
{
	if (Recipient.isEmpty())
		return QVariant();

	int col = index.column();
	int row = index.row();

	if (row < 0 || row >= Dates.size())
		return QVariant();

	ItemCachedData cachedData = fetchCachedData(Dates.at(row));

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (col)
			{
				case 0: return Dates.at(row).toString("dd.MM.yyyy");
				case 1: return cachedData.size;
			}

			return QVariant();
		}

		case HistoryItemRole: return QVariant::fromValue<HistoryTreeItem>(HistoryTreeItem(Recipient));
		case DateRole: return Dates.at(row);
	}

	return QVariant();
}

void SmsDatesModel::setRecipient(const QString &recipient)
{
	Recipient = recipient;
}

void SmsDatesModel::setDates(const QList<QDate> &dates)
{
	Cache->clear();

	beginRemoveRows(QModelIndex(), 0, Dates.size());
	Dates.clear();
	endRemoveRows();

	beginInsertRows(QModelIndex(), 0, dates.size() - 1);
	Dates = dates;
	endInsertRows();
}

QModelIndex SmsDatesModel::indexForDate(const QDate &date)
{
	return index(Dates.indexOf(date));
}
