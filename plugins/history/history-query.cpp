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

#include "history-query.h"

HistoryQuery::HistoryQuery() :
		Limit(0)
{
}

HistoryQuery::HistoryQuery (const HistoryQuery &copyMe)
{
	QueryTalkable = copyMe.QueryTalkable;
	QueryString = copyMe.QueryString;
	FromDate = copyMe.FromDate;
	ToDate = copyMe.ToDate;
	FromDateTime = copyMe.FromDateTime;
	ToDateTime = copyMe.ToDateTime;
	Limit = copyMe.Limit;
}

HistoryQuery & HistoryQuery::operator=(const HistoryQuery &copyMe)
{
	QueryTalkable = copyMe.QueryTalkable;
	QueryString = copyMe.QueryString;
	FromDate = copyMe.FromDate;
	ToDate = copyMe.ToDate;
	FromDateTime = copyMe.FromDateTime;
	ToDateTime = copyMe.ToDateTime;
	Limit = copyMe.Limit;

	return *this;
}

void HistoryQuery::setTalkable(const Talkable &talkable)
{
	QueryTalkable = talkable;
}

Talkable HistoryQuery::talkable() const
{
	return QueryTalkable;
}

void HistoryQuery::setString(const QString &string)
{
	QueryString = string;
}

QString HistoryQuery::string() const
{
	return QueryString;
}

void HistoryQuery::setFromDate(const QDate &fromDate)
{
	FromDate = fromDate;
}

QDate HistoryQuery::fromDate() const
{
	return FromDate;
}

void HistoryQuery::setToDate(const QDate &toDate)
{
	ToDate = toDate;
}

QDate HistoryQuery::toDate() const
{
	return ToDate;
}

void HistoryQuery::setFromDateTime(const QDateTime &fromDateTime)
{
	FromDateTime = fromDateTime;
}

QDateTime HistoryQuery::fromDateTime() const
{
	return FromDateTime;
}

void HistoryQuery::setToDateTime(const QDateTime &toDateTime)
{
	ToDateTime = toDateTime;
}

QDateTime HistoryQuery::toDateTime() const
{
	return ToDateTime;
}

void HistoryQuery::setLimit(quint16 limit)
{
	Limit = limit;
}

quint16 HistoryQuery::limit() const
{
	return Limit;
}
