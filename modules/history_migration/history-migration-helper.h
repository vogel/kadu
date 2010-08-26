/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QList>

class QChar;
class QDateTime;
class QString;
class QStringList;

#define HISTORYMANAGER_ENTRY_CHATSEND   0x00000001
#define HISTORYMANAGER_ENTRY_CHATRCV    0x00000002
#define HISTORYMANAGER_ENTRY_MSGSEND    0x00000004
#define HISTORYMANAGER_ENTRY_MSGRCV     0x00000008
#define HISTORYMANAGER_ENTRY_STATUS     0x00000010
#define HISTORYMANAGER_ENTRY_SMSSEND    0x00000020
#define HISTORYMANAGER_ENTRY_ALL        0x0000003f
#define HISTORYMANAGER_ENTRY_ALL_MSGS   0x0000002f

struct HistoryEntry
{
	int type;
	quint32 uin;
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString message;
	QString status;
	QString ip;
	QString description;
	QString mobile;
};

namespace HistoryMigrationHelper
{
	QString getFileNameByUinsList(QStringList uins);
	int getHistoryEntriesCount(const QStringList &uins);
	QList<QStringList> getUinsLists();
	QList<HistoryEntry> historyEntries(QStringList uins, int mask);
	QStringList mySplit(const QChar &sep, const QString &str);
};

#endif // HISTORY_MIGRATION_HELPER_H
