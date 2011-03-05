/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_MIGRATION_HELPER_H
#define HISTORY_MIGRATION_HELPER_H

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>

class QChar;
class QStringList;

typedef quint32 UinType;
typedef QList<UinType> UinsList;

struct HistoryEntry
{
	// flag
	enum EntryType
	{
		EntryNone      = 0x0000,
		ChatSend       = 0x0001,
		ChatRcv        = 0x0002,
		MsgSend        = 0x0004,
		MsgRcv         = 0x0008,
		StatusChange   = 0x0010,
		SmsSend        = 0x0020
	};

	enum StatusType
	{
		StatusNone,
		Online,
		Busy,
		Invisible,
		FFC,
		DND,
		Offline,
		StatusLast = Offline
	};

	EntryType Type;
	UinType Uin;
	QString Nick;
	QDateTime Date;
	QDateTime SendDate;
	QString Content;
	StatusType Status;
	QString Mobile;

	HistoryEntry() : Type(EntryNone), Uin(0), Status(StatusNone) {}
};

namespace HistoryMigrationHelper
{
	QString getFileNameByUinsList(UinsList uins);
	int getHistoryEntriesCount(const QString &path, const UinsList &uins);
	QList<UinsList> getUinsLists(const QString &path);
	QList<HistoryEntry> historyEntries(const QString &path, const UinsList &uins);
	QStringList mySplit(const QChar &sep, const QString &str);
}

#endif // HISTORY_MIGRATION_HELPER_H
