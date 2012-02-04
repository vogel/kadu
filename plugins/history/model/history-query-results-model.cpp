/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "chat/chat.h"
#include "model/roles.h"

#include "history-query-result.h"
#include "history.h"

#include "history-query-results-model.h"

HistoryQueryResultsModel::HistoryQueryResultsModel(QObject *parent) :
		QAbstractListModel(parent)
{
	TalkableHeader = tr("Chat");
	LengthHeader = tr("Length");
}

HistoryQueryResultsModel::~HistoryQueryResultsModel()
{
}

int HistoryQueryResultsModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 4;
}

int HistoryQueryResultsModel::rowCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : Results.size();
}

QVariant HistoryQueryResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation != Qt::Horizontal)
		return QVariant();

	switch (section)
	{
		case 0: return TalkableHeader;
		case 1: return tr("Date");
		case 2: return LengthHeader;
		case 3: return tr("Title");
	}

	return QVariant();
}

QVariant HistoryQueryResultsModel::data(const QModelIndex &index, int role) const
{
	int col = index.column();
	int row = index.row();

	if (row < 0 || row >= Results.size())
		return QVariant();

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (col)
			{
				case 0: return Results.at(row).talkable().display();
				case 1: return Results.at(row).date().toString("dd.MM.yyyy");
				case 2: return Results.at(row).count();
				case 3: return Results.at(row).title();
			}

			return QVariant();
		}

		case DateRole: return Results.at(row).date();
		case TalkableRole: return QVariant::fromValue(Results.at(row).talkable());
	}

	return QVariant();
}

void HistoryQueryResultsModel::setTalkableHeader(const QString &talkableHeader)
{
	if (TalkableHeader == talkableHeader)
		return;

	TalkableHeader = talkableHeader;
	emit headerDataChanged(Qt::Horizontal, 0, 0);
}

void HistoryQueryResultsModel::setLengthHeader (const QString &lengthHeader)
{
	if (LengthHeader == lengthHeader)
		return;

	LengthHeader = lengthHeader;
	emit headerDataChanged(Qt::Horizontal, 3, 3);
}

void HistoryQueryResultsModel::setResults(const QVector<HistoryQueryResult> &results)
{
	beginResetModel();
	Results = results;
	endResetModel();
}
