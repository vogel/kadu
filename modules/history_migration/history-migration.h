#ifndef HISTORY_IMPORTER_H
#define HISTORY_IMPORTER_H

#include "status/status.h"

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
	UinType uin;
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString message;
	Status::StatusType status;
	QString ip;
	QString description;
	QString mobile;
};

class HistoryImporter
{
	QList<QStringList> getUinsLists() const;
	QString getFileNameByUinsList(QStringList uins);
	int getHistoryEntriesCountPrivate(const QString &filename) const;
	int getHistoryEntriesCount(const QStringList &uins);
	QList<HistoryEntry> getHistoryEntries(QStringList uins, int from, int count, int mask);
	QStringList mySplit(const QChar &sep, const QString &str);



public:
	HistoryImporter();
	~HistoryImporter();
};

extern HistoryImporter* historyImporter;

#endif //HISTORY_IMPORTER_H
