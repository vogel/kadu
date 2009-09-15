/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "history-search-parameters.h"

HistorySearchParameters::HistorySearchParameters()
{
}

HistorySearchParameters & HistorySearchParameters::setQuery(const QString &query)
{
	Query = query;
	return *this;
}

HistorySearchParameters & HistorySearchParameters::setFromDate(const QDate &date)
{
	FromDate = date;
	return *this;
}

HistorySearchParameters & HistorySearchParameters::setToDate(const QDate &date)
{
	ToDate = date;
	return *this;
}
