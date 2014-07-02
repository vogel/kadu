/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef HISTORY_QUERY_H
#define HISTORY_QUERY_H

#include <QtCore/QDate>

#include "talkable/talkable.h"

#include "history_exports.h"

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryQuery
 * @author Rafał 'Vogel' Malinowski
 * @short This class represents query for messages in history.
 *
 * This class represents query for messages in history. Each query can contain a talkable,
 * query string, range of dates (or datetimes) and limit. Any query element can be empty - it will be ignored during
 * search. Any query element can be ignored by given history method. This behavior is documented
 * for these methods.
 *
 * Results of query are stored in QVector of HistoryQueryResult.
 */
class HISTORYAPI HistoryQuery
{
	Talkable QueryTalkable;
	QString QueryString;
	QDate FromDate;
	QDate ToDate;
	QDateTime FromDateTime;
	QDateTime ToDateTime;
	int Limit;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create empty query.
	 */
	HistoryQuery();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create copy of existing query.
	 * @param copyMe existing query to copy
	 */
	HistoryQuery(const HistoryQuery &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Copy existing query into this query.
	 * @param copyMe existing query to copy
	 */
	HistoryQuery & operator = (const HistoryQuery &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets talkable filter.
	 * @param talkable talkable to filter
	 *
	 * Only items originating from this talkable will be returned.
	 */
	void setTalkable(const Talkable &talkable);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return talkable filter.
	 * @return talkable filter
	 */
	Talkable talkable() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets query string filter.
	 * @param string query string to filter
	 *
	 * Only items with content containing this string will be returned.
	 */
	void setString(const QString &string);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return query string filter.
	 * @return query string filter
	 */
	QString string() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets from date filter.
	 * @param fromDate from date filter
	 *
	 * Only items with content added after this date (inclusive) will be returned.
	 */
	void setFromDate(const QDate &fromDate);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return from date filter.
	 * @return from date filter
	 */
	QDate fromDate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets to date filter.
	 * @param toDate to date filter
	 *
	 * Only items with content added before this date (inclusive) will be returned.
	 */
	void setToDate(const QDate &toDate);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return to date filter.
	 * @return to date filter
	 */
	QDate toDate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets from date filter.
	 * @param fromDateTime from date filter
	 *
	 * Only items with content added after this date (inclusive) will be returned.
	 */
	void setFromDateTime(const QDateTime &fromDateTime);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return from date filter.
	 * @return from date filter
	 */
	QDateTime fromDateTime() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets to date filter.
	 * @param toDateTime to date filter
	 *
	 * Only items with content added before this date (inclusive) will be returned.
	 */
	void setToDateTime(const QDateTime &toDateTime);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return to date filter.
	 * @return to date filter
	 */
	QDateTime toDateTime() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets limit.
	 * @param limit limit
	 *
	 * Maximum limit items will be returned.
	 */
	void setLimit(int limit);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return limit.
	 * @return limit
	 */
	int limit() const;

};

/**
 * @}
 */

#endif //  HISTORY_QUERY_H
