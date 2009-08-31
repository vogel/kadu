/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_IMPORT_THREAD_H
#define HISTORY_IMPORT_THREAD_H

#include <QtCore/QStringList>
#include <QtCore/QThread>

class Account;
class Chat;

struct HistoryEntry;

class HistoryImportThread : public QThread
{
	Q_OBJECT

	Account *GaduAccount;
	QList<QStringList> UinsLists;

	bool Canceled;
	unsigned long TotalEntries;
	unsigned long ImportedEntries;

	Chat * chatFromUinsList(QStringList uinsList);
	QList<HistoryEntry> historyEntries(QStringList uins, int mask);
	void importEntry(Chat *chat, const HistoryEntry &entry);
	QStringList mySplit(const QChar &sep, const QString &str);

public:
	HistoryImportThread(Account *gaduAccount, QList<QStringList> uinsLists, unsigned long totalEntries, QObject *parent = 0);
	virtual ~HistoryImportThread();

	static QString getFileNameByUinsList(QStringList uins);

	virtual void run();
	void cancel();

	unsigned long importedEntries() { return ImportedEntries; }

};

#endif // HISTORY_IMPORT_THREAD_H
