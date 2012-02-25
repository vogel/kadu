/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HISTORY_IMPORT_THREAD_H
#define HISTORY_IMPORT_THREAD_H

#include <QtCore/QList>
#include <QtCore/QObject>

#include "accounts/account.h"

#include "history-migration-helper.h"

class Chat;
struct HistoryEntry;

class HistoryImportThread : public QObject
{
	Q_OBJECT

	Account GaduAccount;
	QString Path;
	QList<UinsList> UinsLists;

	int TotalEntries;
	int ImportedEntries;
	int ImportedChats;
	int TotalMessages;
	int ImportedMessages;

	bool Canceled;
	bool CancelForced;

	Chat chatFromUinsList(const UinsList &uinsList) const;
	void importEntry(const Chat &chat, const HistoryEntry &entry);

public:
	HistoryImportThread(Account gaduAccount, const QString &path, const QList<UinsList> &uinsLists, int totalEntries, QObject *parent = 0);
	virtual ~HistoryImportThread();

	void prepareChats();

	int importedEntries() { return ImportedEntries; }
	int importedChats() { return ImportedChats; }
	int totalMessages() { return TotalMessages; }
	int importedMessages() { return ImportedMessages; }

	bool wasCanceled() { return Canceled; }

public slots:
	void run();
	void cancel(bool force = false);

signals:
	void finished();

};

#endif // HISTORY_IMPORT_THREAD_H
