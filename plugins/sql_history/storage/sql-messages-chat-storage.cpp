/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "message/sorted-messages.h"
#include "storage/history-sql-storage.h"

#include "sql-messages-chat-storage.h"

SqlMessagesChatStorage::SqlMessagesChatStorage(HistorySqlStorage *sqlStorage) :
		HistoryMessagesStorage(sqlStorage), SqlStorage(sqlStorage)
{
	Q_ASSERT(SqlStorage);
}

SqlMessagesChatStorage::~SqlMessagesChatStorage()
{
}

QFuture<QVector<Talkable>> SqlMessagesChatStorage::talkables()
{
	return SqlStorage->chats();
}

QFuture<QVector<HistoryQueryResult>> SqlMessagesChatStorage::dates(const HistoryQuery &historyQuery)
{
	return SqlStorage->chatDates(historyQuery);
}

QFuture<SortedMessages> SqlMessagesChatStorage::messages(const HistoryQuery &historyQuery)
{
	return SqlStorage->messages(historyQuery);
}

void SqlMessagesChatStorage::deleteMessages(const Talkable &talkable, const QDate &date)
{
	SqlStorage->clearChatHistory(talkable, date);
}

#include "moc_sql-messages-chat-storage.cpp"
