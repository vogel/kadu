/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_QUERY_RESULT_H
#define HISTORY_QUERY_RESULT_H

#include <QtCore/QDate>

#include "talkable/talkable.h"

#include "history_exports.h"

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryQueryResult
 * @author Rafał 'Vogel' Malinowski
 * @short This class represents one result item of query for messages in history.
 *
 * This class represents one result item of query for messages in history. Each results consists
 * of originating talkable item, date of this item, title (that is in general content value of first
 * item in given day) and count of found items for given date.
 */
class HISTORYAPI HistoryQueryResult
{
	Talkable ResultTalkable;
	QDate Date;
	QString Title;
	quint32 Count;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create empty query result.
	 */
	HistoryQueryResult();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create copy of existing query result.
	 * @param copyMe existing query result to copy
	 */
	HistoryQueryResult(const HistoryQueryResult &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Copy existing query resylt into this query result.
	 * @param copyMe existing query result to copy
	 */
	HistoryQueryResult & operator = (const HistoryQueryResult &copyMe);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets originating talkable.
	 * @param talkable originating talkable
	 */
	void setTalkable(const Talkable &talkable);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns originating talkable.
	 * @return originating talkable
	 */
	Talkable talkable() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets date of query result.
	 * @param date of query result
	 */
	void setDate(const QDate &date);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns date of query result.
	 * @return date of query result
	 */
	QDate date() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets title of query result.
	 * @param title title of query result
	 */
	void setTitle(const QString &title);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns title of query result.
	 * @return title of query result
	 */
	QString title() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Sets count of found items.
	 * @param count count of found items
	 */
	void setCount(const quint32 count);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns count of found items.
	 * @return count of found items
	 */
	quint32 count() const;

};

/**
 * @}
 */

Q_DECLARE_METATYPE(HistoryQueryResult);

#endif // HISTORY_QUERY_RESULT_H
