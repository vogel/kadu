/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "history-query-results-proxy-model.h"

HistoryQueryResultsProxyModel::HistoryQueryResultsProxyModel(QObject *parent) :
		QSortFilterProxyModel(parent), TalkableVisible(true), TitleVisible(true)
{
}

HistoryQueryResultsProxyModel::~HistoryQueryResultsProxyModel()
{
}

bool HistoryQueryResultsProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const
{
	Q_UNUSED(sourceParent);

	if (!TalkableVisible && 0 == sourceColumn)
		return false;
	if (!TitleVisible && 3 == sourceColumn)
		return false;

	return true;
}

void HistoryQueryResultsProxyModel::setTalkableVisible(const bool talkableVisible)
{
	if (TalkableVisible == talkableVisible)
		return;

	TalkableVisible = talkableVisible;
	invalidateFilter();
}

void HistoryQueryResultsProxyModel::setTitleVisible(const bool titleVisible)
{
	if (TitleVisible == titleVisible)
		return;

	TitleVisible = titleVisible;
	invalidateFilter();
}
