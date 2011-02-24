/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
	const QString & query() const { return Query; }

	HistorySearchParameters & setFromDate(const QDate &date);
	const QDate & fromDate() const { return FromDate; }

	HistorySearchParameters & setToDate(const QDate &date);
	const QDate & toDate() const { return ToDate; }

};

#endif // HISTORY_SEARCH_PARAMETERS_H
