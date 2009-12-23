/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFile>

#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/message/message.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "misc/misc.h"
#include "modules/history/history.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "history-import-thread.h"

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

HistoryImportThread::HistoryImportThread(Account gaduAccount, QList<QStringList> uinsLists, unsigned long totalEntries, QObject *parent) :
		QThread(parent), GaduAccount(gaduAccount), UinsLists(uinsLists), Canceled(false), TotalEntries(totalEntries), ImportedEntries(0)
{
}

HistoryImportThread::~HistoryImportThread()
{
}

void HistoryImportThread::run()
{
	ImportedEntries = 0;

	foreach (QStringList uinsList, UinsLists)
	{
		Chat chat = chatFromUinsList(uinsList);
		if (!chat)
			continue;

		QList<HistoryEntry> entries = historyEntries(uinsList, HISTORYMANAGER_ENTRY_CHATSEND | HISTORYMANAGER_ENTRY_CHATRCV);

		foreach (const HistoryEntry &entry, entries)
			if (!Canceled)
				importEntry(chat, entry);
	}
}

void HistoryImportThread::cancel()
{
	Canceled = true;
}

void HistoryImportThread::importEntry(Chat chat, const HistoryEntry &entry)
{
	QString id = QString::number(entry.uin);

	bool outgoing = entry.type == HISTORYMANAGER_ENTRY_CHATSEND;
	QDateTime sendTime = entry.sdate;
	QDateTime recieveTime = entry.date;
	QString messageString = entry.message;

	Message msg = Message::create();
	msg.setMessageChat(chat);
	msg.setMessageSender(outgoing
			? GaduAccount.accountContact()
			: ContactManager::instance()->byId(GaduAccount, id, true));
	msg.setContent(entry.message);
	msg.setSendDate(entry.sdate);
	msg.setReceiveDate(entry.date);

	//TODO 0.6.6: it's damn slow!
	History::instance()->currentStorage()->appendMessage(msg);
	ImportedEntries++;
}

Chat HistoryImportThread::chatFromUinsList(QStringList uinsList)
{
	ContactSet contacts;
	foreach (const QString &uin, uinsList)
		contacts.insert(ContactManager::instance()->byId(GaduAccount, uin, true));

	return ChatManager::instance()->findChat(contacts);
}

QList<HistoryEntry> HistoryImportThread::historyEntries(QStringList uins, int mask)
{
	kdebugf();

	QList<HistoryEntry> entries;
	QStringList tokens;
	QFile f, fidx;
	QString path = ggPath("history/");
	QString filename, line;
	int offs;

	if (!uins.isEmpty())
		filename = getFileNameByUinsList(uins);
	else
		filename = "sms";
	f.setFileName(path + filename);
	if (!f.open(QIODevice::ReadOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", qPrintable(filename));
		return entries;
	}

	fidx.setFileName(f.fileName() + ".idx");
	if (!fidx.open(QIODevice::ReadOnly))
		return entries;
	fidx.seek(0);
	fidx.read((char *)&offs, sizeof(int));
	fidx.close();
	if (!f.seek(offs))
		return entries;

	QTextStream stream(&f);
	stream.setCodec("CP1250");

	int linenr = 0;

	struct HistoryEntry entry;
	//	int num = 0;
	while ((line = stream.readLine()) != QString::null)
	{
		++linenr;
		tokens = mySplit(',', line);
		if (tokens.count() < 2)
			continue;
		if (tokens[0] == "chatsend")
			entry.type = HISTORYMANAGER_ENTRY_CHATSEND;
		else if (tokens[0] == "msgsend")
			entry.type = HISTORYMANAGER_ENTRY_MSGSEND;
		else if (tokens[0] == "chatrcv")
			entry.type = HISTORYMANAGER_ENTRY_CHATRCV;
		else if (tokens[0] == "msgrcv")
			entry.type = HISTORYMANAGER_ENTRY_MSGRCV;
		else if (tokens[0] == "status")
			entry.type = HISTORYMANAGER_ENTRY_STATUS;
		else if (tokens[0] == "smssend")
			entry.type = HISTORYMANAGER_ENTRY_SMSSEND;
		if (!(entry.type & mask))
			continue;
		//		if (num++%10==0)
		//			qApp->processEvents();
		switch (entry.type)
		{
			case HISTORYMANAGER_ENTRY_CHATSEND:
			case HISTORYMANAGER_ENTRY_MSGSEND:
				if (tokens.count() == 5)
				{
					entry.uin = tokens[1].toUInt();
					entry.nick = tokens[2];
					entry.date.setTime_t(tokens[3].toUInt());
					entry.message = tokens[4];
					entry.ip.truncate(0);
					entry.mobile.truncate(0);
					entry.description.truncate(0);
					entries.append(entry);
				}
				break;
			case HISTORYMANAGER_ENTRY_CHATRCV:
			case HISTORYMANAGER_ENTRY_MSGRCV:
				if (tokens.count() == 6)
				{
					entry.uin = tokens[1].toUInt();
					entry.nick = tokens[2];
					entry.date.setTime_t(tokens[3].toUInt());
					entry.sdate.setTime_t(tokens[4].toUInt());
					entry.message = tokens[5];
					entry.ip.truncate(0);
					entry.mobile.truncate(0);
					entry.description.truncate(0);
					entries.append(entry);
				}
				break;
			case HISTORYMANAGER_ENTRY_STATUS:
				if (tokens.count() == 6 || tokens.count() == 7)
				{
					entry.uin = tokens[1].toUInt();
					entry.nick = tokens[2];
					entry.ip = tokens[3];
					entry.date.setTime_t(tokens[4].toUInt());
					if (tokens[5] == "avail")
						entry.status = "Online";
					else if (tokens[5] == "notavail")
						entry.status = "Offline";
					else if (tokens[5] == "busy")
						entry.status = "Busy";
					else if (tokens[5] == "invisible")
						entry.status = "Invisible";
					if (tokens.count() == 7)
						entry.description = tokens[6];
					else
						entry.description.truncate(0);
					entry.mobile.truncate(0);
					entry.message.truncate(0);
					entries.append(entry);
				}
				break;
			case HISTORYMANAGER_ENTRY_SMSSEND:
				if (tokens.count() == 4 || tokens.count() == 6)
				{
					entry.mobile = tokens[1];
					entry.date.setTime_t(tokens[2].toUInt());
					entry.message = tokens[3];
					if (tokens.count() == 4)
					{
						entry.nick.truncate(0);
						entry.uin = 0;
					}
					else
					{
						entry.nick = tokens[4];
						entry.uin = tokens[5].toUInt();
					}
					entry.ip.truncate(0);
					entry.description.truncate(0);
					entries.append(entry);
				}
				break;
		}
	}

	f.close();

	kdebugf2();
	return entries;
}

QStringList HistoryImportThread::mySplit(const QChar &sep, const QString &str)
{
	kdebugf();
	QStringList strlist;
	QString token;
	unsigned int idx = 0, strlength = str.length();
	bool inString = false;

	int pos1, pos2;
	while (idx < strlength)
	{
		const QChar &letter = str[idx];
		if (inString)
		{
			if (letter == '\\')
			{
				switch (str[idx + 1].toAscii())
				{
					case 'n':
						token.append('\n');
						break;
					case '\\':
						token.append('\\');
						break;
					case '\"':
						token.append('"');
						break;
					default:
						token.append('?');
				}
				idx += 2;
			}
			else if (letter == '"')
			{
				strlist.append(token);
				inString = false;
				++idx;
			}
			else
			{
				pos1 = str.indexOf('\\', idx);
				if (pos1 == -1)
					pos1 = strlength;
				pos2 = str.indexOf('"', idx);
				if (pos2 == -1)
					pos2 = strlength;
				if (pos1 < pos2)
				{
					token.append(str.mid(idx, pos1 - idx));
					idx = pos1;
				}
				else
				{
					token.append(str.mid(idx, pos2 - idx));
					idx = pos2;
				}
			}
		}
		else // out of the string
		{
			if (letter == sep)
			{
				if (!token.isEmpty())
					token = QString::null;
				else
					strlist.append(QString::null);
			}
			else if (letter == '"')
				inString = true;
			else
			{
				pos1 = str.indexOf(sep, idx);
				if (pos1 == -1)
					pos1 = strlength;
				token.append(str.mid(idx, pos1 - idx));
				strlist.append(token);
				idx = pos1;
				continue;
			}
			++idx;
		}
	}

	kdebugf2();
	return strlist;
}

QString HistoryImportThread::getFileNameByUinsList(QStringList uins)
{
	kdebugf();

	if (uins.isEmpty())
		return "sms";

	QString fname;
	uins.sort();
	unsigned int i = 0, uinsCount = uins.count();
	foreach (const QString &uin, uins)
	{
		fname.append(uin);
		if (i++ < uinsCount - 1)
			fname.append("_");
	}

	return fname;
}
