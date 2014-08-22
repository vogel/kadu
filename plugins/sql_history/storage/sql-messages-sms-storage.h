/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SQL_MESSAGES_SMS_STORAGE_H
#define SQL_MESSAGES_SMS_STORAGE_H

#include "plugins/history/storage/history-messages-storage.h"

class HistorySqlStorage;

class SqlMessagesSmsStorage : public HistoryMessagesStorage
{
	Q_OBJECT

	HistorySqlStorage *SqlStorage;

public:
	explicit SqlMessagesSmsStorage(HistorySqlStorage *sqlStorage);
	virtual ~SqlMessagesSmsStorage();

	virtual QFuture<QVector<Talkable>> talkables();
	virtual QFuture<QVector<HistoryQueryResult>> dates(const HistoryQuery &historyQuery);
	virtual QFuture<SortedMessages> messages(const HistoryQuery &historyQuery);

	virtual void deleteMessages(const Talkable &talkable, const QDate &date = QDate());

};

#endif // SQL_MESSAGES_SMS_STORAGE_H
