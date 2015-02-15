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

#ifndef HISTORY_MESSAGES_STORAGE_H
#define HISTORY_MESSAGES_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QFuture>
#include <QtCore/QObject>

#include "message/message.h"
#include "talkable/talkable.h"

#include "../history-query-result.h"
#include "../history_exports.h"

class HistoryQuery;
class SortedMessages;

/**
 * @addtogroup History
 * @{
 */

/**
 * @class HistoryMessagesStorage
 * @author Rafał 'Vogel' Malinowski
 * @short Interface that provides read and delete access to a message storage.
 * @todo Think about adding writing here.
 *
 * This interface provides read and delete access to a message storage. A message storage
 * contains list of messages. Each message has is connected with a data and belongs to given
 * talkable. List of available talkables can be fetched using talkables() method. List of available
 * dates for given talkables can be fetched using dates() method. List of messages for given
 * talkable and date can be fetched using messages() method.
 *
 * All messages for given talkable can be deleted by deleteMessages() method. Is also allows deleting
 * messages for given talkable and date.
 */
class HISTORYAPI HistoryMessagesStorage : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new HistoryMessagesStorage instance.
	 * @param parent QObject parent of new HistoryMessagesStorage instance
	 */
	explicit HistoryMessagesStorage(QObject *parent) : QObject(parent) {}
	virtual ~HistoryMessagesStorage() {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of available talkables.
	 * @return list of available talkables
	 *
	 * This methods returns list of available talkables in asynchronous way. Refer to QFuture
	 * documentation for more information.
	 */
	virtual QFuture<QVector<Talkable>> talkables() = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of available dates for given query.
	 * @param historyQuery query to get list of dates for
	 * @return list of available dates for given query
	 *
	 * This methods returns list of available dates for given talkabel in asynchronous way. Refer to QFuture
	 * documentation for more information. Limit query parametr is ignored for this method.
	 */
	virtual QFuture<QVector<HistoryQueryResult>> dates(const HistoryQuery &historyQuery) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Returns list of messages for given talkable and data.
	 * @param historyQuery query to get list of messages for
	 * @return list of messages for given query
	 *
	 * This methods returns list of message in asynchronous way. Refer to QFuture
	 * documentation for more information.
	 *
	 * Query string parameter is ignored for this method. Limit query parameter can be ignored in
	 * implementations of this method.
	 */
	virtual QFuture<SortedMessages> messages(const HistoryQuery &historyQuery) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Delete all messages for given talkable and data.
	 * @param talkable talkable to delete list of messages for
	 * @param date date to delete list of messages for
	 *
	 * This methods removes all messages for given talkable and date. If no date is provided
	 * all messages for give talkable are removed.
	 */
	virtual void deleteMessages(const Talkable &talkable, const QDate &date = QDate()) = 0;

};

/**
 * @}
 */

#endif // HISTORY_MESSAGES_STORAGE_H
