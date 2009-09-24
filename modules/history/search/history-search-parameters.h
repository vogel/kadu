/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_SEARCH_PARAMETERS_H
#define HISTORY_SEARCH_PARAMETERS_H

#include <QtCore/QDate>
#include <QtCore/QString>

class HistorySearchParameters
{
	QString Query;
	QDate FromDate;
	QDate ToDate;

public:
	HistorySearchParameters();

	HistorySearchParameters & setQuery(const QString &query);
	QString query() { return Query; }

	HistorySearchParameters & setFromDate(const QDate &date);
	QDate fromDate() { return FromDate; }
	
	HistorySearchParameters & setToDate(const QDate &date);
	QDate toDate() { return ToDate; }

};

#endif // HISTORY_SEARCH_PARAMETERS_H
