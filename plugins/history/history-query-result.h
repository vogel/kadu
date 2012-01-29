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

#ifndef HISTORY_QUERY_RESULT_H
#define HISTORY_QUERY_RESULT_H

#include <QtCore/QDate>

#include "talkable/talkable.h"

class HistoryQueryResult
{
	Talkable ResultTalkable;
	QDate Date;
	QString Title;
	quint32 Count;

public:
	HistoryQueryResult();
	HistoryQueryResult(const HistoryQueryResult &copyMe);

	HistoryQueryResult & operator = (const HistoryQueryResult &copyMe);

	void setTalkable(const Talkable &talkable);
	Talkable talkable() const;

	void setDate(const QDate &date);
	QDate date() const;

	void setTitle(const QString &title);
	QString title() const;

	void setCount(const quint32 count);
	quint32 count() const;

};

Q_DECLARE_METATYPE(HistoryQueryResult);

#endif // HISTORY_QUERY_RESULT_H
