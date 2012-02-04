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

#include "history-query-result.h"

HistoryQueryResult::HistoryQueryResult() :
		Count(0)
{
}

HistoryQueryResult::HistoryQueryResult(const HistoryQueryResult &copyMe)
{
	ResultTalkable = copyMe.ResultTalkable;
	Date = copyMe.Date;
	Title = copyMe.Title;
	Count = copyMe.Count;
}

HistoryQueryResult & HistoryQueryResult::operator = (const HistoryQueryResult &copyMe)
{
	ResultTalkable = copyMe.ResultTalkable;
	Date = copyMe.Date;
	Title = copyMe.Title;
	Count = copyMe.Count;

	return *this;
}

void HistoryQueryResult::setTalkable(const Talkable &talkable)
{
	ResultTalkable = talkable;
}

Talkable HistoryQueryResult::talkable() const
{
	return ResultTalkable;
}

void HistoryQueryResult::setDate(const QDate &date)
{
	Date = date;
}

QDate HistoryQueryResult::date() const
{
	return Date;
}

void HistoryQueryResult::setTitle(const QString &title)
{
	Title = title;
}

QString HistoryQueryResult::title() const
{
	return Title;
}

void HistoryQueryResult::setCount(const quint32 count)
{
	Count = count;
}

quint32 HistoryQueryResult::count() const
{
	return Count;
}
