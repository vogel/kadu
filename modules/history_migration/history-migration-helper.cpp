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

#include <QtCore/QByteArray>
#include <QtCore/QChar>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>

#include "misc/misc.h"
#include "misc/path-conversion.h"
#include "debug.h"

#include "history-migration-helper.h"

namespace HistoryMigrationHelper
{
	QString getFileNameByUinsList(QStringList uins)
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

	int getHistoryEntriesCount(const QStringList &uins)
	{
		kdebugf();

		int lines;
		QFile f;
		QString filename = getFileNameByUinsList(uins);
		QString path = profilePath("history/");
		QByteArray buffer;

		f.setFileName(path + filename + ".idx");
		if (!f.open(QIODevice::ReadOnly))
		{
			kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", qPrintable(filename));
			kdebugf2();
			return 0;
		}
		lines = f.size() / sizeof(int);

		f.close();

		kdebugmf(KDEBUG_INFO, "%d lines\n", lines);
		kdebugf2();
		return lines;
	}

	QList<QStringList> getUinsLists()
	{
		kdebugf();
		QList<QStringList> entries;
		QDir dir(profilePath("history/"), "*.idx");
		QStringList struins;
		QStringList uins;

		foreach (QString entry, dir.entryList())
		{
			struins = entry.remove(QRegExp(".idx$")).split("_", QString::SkipEmptyParts);
			uins.clear();
			if (struins[0] != "sms")
				foreach (const QString &struin, struins)
					uins.append(struin);
			entries.append(uins);
		}

		kdebugf2();
		return entries;
	}

	QList<HistoryEntry> historyEntries(QStringList uins, int mask)
	{
		kdebugf();

		QList<HistoryEntry> entries;
		QStringList tokens;
		QFile f, fidx;
		QString path = profilePath("history/");
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
		stream.setCodec(codec_latin2);

		int linenr = 0;

		//	int num = 0;
		while ((line = stream.readLine()) != QString::null)
		{
			HistoryEntry entry;

			++linenr;
			tokens = mySplit(',', line);
			if (tokens.count() < 2)
				continue;
			if (tokens[0] == "chatsend")
				entry.Type = HistoryEntry::ChatSend;
			else if (tokens[0] == "msgsend")
				entry.Type = HistoryEntry::MsgSend;
			else if (tokens[0] == "chatrcv")
				entry.Type = HistoryEntry::ChatRcv;
			else if (tokens[0] == "msgrcv")
				entry.Type = HistoryEntry::MsgRcv;
			else if (tokens[0] == "status")
				entry.Type = HistoryEntry::StatusChange;
			else if (tokens[0] == "smssend")
				entry.Type = HistoryEntry::SmsSend;
			if (!(entry.Type & mask))
				continue;
			//		if (num++%10==0)
			//			qApp->processEvents();
			switch (entry.Type)
			{
				case HistoryEntry::ChatSend:
				case HistoryEntry::MsgSend:
					if (tokens.count() == 5)
					{
						entry.Uin = tokens[1].toUInt();
						entry.Nick = tokens[2];
						entry.Date.setTime_t(tokens[3].toUInt());
						entry.Message = tokens[4];
						entries.append(entry);
					}
					break;
				case HistoryEntry::ChatRcv:
				case HistoryEntry::MsgRcv:
					if (tokens.count() == 6)
					{
						entry.Uin = tokens[1].toUInt();
						entry.Nick = tokens[2];
						entry.Date.setTime_t(tokens[3].toUInt());
						entry.SendDate.setTime_t(tokens[4].toUInt());
						entry.Message = tokens[5];
						entries.append(entry);
					}
					break;
				case HistoryEntry::StatusChange:
					if (tokens.count() == 6 || tokens.count() == 7)
					{
						entry.Uin = tokens[1].toUInt();
						entry.Nick = tokens[2];
						entry.Ip = tokens[3];
						entry.Date.setTime_t(tokens[4].toUInt());
						if (tokens[5] == "avail")
							entry.Status = HistoryEntry::Online;
						else if (tokens[5] == "notavail")
							entry.Status = HistoryEntry::Offline;
						else if (tokens[5] == "busy")
							entry.Status = HistoryEntry::Busy;
						else if (tokens[5] == "invisible")
							entry.Status = HistoryEntry::Invisible;
						if (tokens.count() == 7)
							entry.Description = tokens[6];
						entries.append(entry);
					}
					break;
				case HistoryEntry::SmsSend:
					if (tokens.count() == 4 || tokens.count() == 6)
					{
						entry.Mobile = tokens[1];
						entry.Date.setTime_t(tokens[2].toUInt());
						entry.Message = tokens[3];
						if (tokens.count() == 6)
						{
							entry.Nick = tokens[4];
							entry.Uin = tokens[5].toUInt();
						}
						entries.append(entry);
					}
					break;
				default:
					break;
			}
		}

		f.close();

		kdebugf2();
		return entries;
	}

	QStringList mySplit(const QChar &sep, const QString &str)
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
};
