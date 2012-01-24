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

#ifndef HISTORY_MESSAGES_STORAGE_H
#define HISTORY_MESSAGES_STORAGE_H

#include <QtCore/QDate>
#include <QtCore/QFuture>
#include <QtCore/QObject>

#include "message/message.h"
#include "talkable/talkable.h"

#include "plugins/history/history_exports.h"
#include "plugins/history/model/dates-model-item.h"

class HISTORYAPI HistoryMessagesStorage : public QObject
{
	Q_OBJECT

public:
	explicit HistoryMessagesStorage(QObject *parent) : QObject(parent) {}
	virtual ~HistoryMessagesStorage() {}

	virtual QFuture<QVector<Talkable> > talkables() = 0;
	virtual QFuture<QVector<DatesModelItem> > dates(const Talkable &talkable) = 0;
	virtual QFuture<QVector<Message> > messages(const Talkable &talkable, const QDate &date) = 0;

	virtual void deleteMessages(const Talkable &talkable, const QDate &date = QDate()) = 0;

};

#endif // HISTORY_MESSAGES_STORAGE_H
