/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QByteArray>
#include <QtCore/QChar>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QtAlgorithms>

#include "misc/misc.h"
#include "debug.h"

#include "history-migration-helper.h"

namespace HistoryMigrationHelper
{
	QString getFileNameByUinsList(UinsList uins)
	{
		kdebugf();

		if (uins.isEmpty())
			return "sms";

		qSort(uins);
		QString fname;
		foreach (UinType uin, uins)
			fname.append(QString::number(uin) + '_');
		fname.remove(fname.length() - 1, 1);

		return fname;
	}

	int getHistoryEntriesCount(const QString &path, const UinsList &uins)
	{
		kdebugf();

		int lines = 0;
		QString filename = getFileNameByUinsList(uins);
		QByteArray buffer;
		QFile f(path + filename);

		if (f.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&f);
			while (!stream.readLine().isNull())
				lines++;
			f.close();
		}
		else
		{
			kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", qPrintable(filename));
			kdebugf2();
			return 0;
		}

		kdebugmf(KDEBUG_INFO, "%d lines\n", lines);
		kdebugf2();
		return lines;
	}

	QList<UinsList> getUinsLists(const QString &path)
	{
		kdebugf();
		QList<UinsList> entries;
		QDir dir(path);
		UinsList uins;

		QRegExp historyEntryRegExp("[0-9]+(_[0-9]+)*");

		foreach (const QString &entry, dir.entryList())
		{
			if (!historyEntryRegExp.exactMatch(entry))
				continue;

			uins.clear();

			QStringList struins = entry.split('_', QString::SkipEmptyParts);
			bool ok;
			foreach (const QString &struin, struins)
			{
				uins.append(struin.toUInt(&ok));
				if (!ok)
					break;
			}
			if (ok)
				entries.append(uins);
		}

		// special case for sms: probably it won't have an index file,
		// so only check if sms itself exists and append empty uins list
		if (QFile::exists(path + "/sms"))
		{
			uins.clear();
			entries.append(uins);
		}

		kdebugf2();
		return entries;
	}

	QList<HistoryEntry> historyEntries(const QString &path, const UinsList &uins)
	{
		kdebugf();

		QList<HistoryEntry> entries;
		QStringList tokens;
		QFile f;
		QString filename, line;

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

		QTextStream stream(&f);
		stream.setCodec(codec_latin2);
		while (!(line = stream.readLine()).isNull())
		{
			HistoryEntry entry;

			// because of a bug in Kadu 0.6.5 or Qt4 sometimes (very rarely)
			// garbage is put before entry type string
			QRegExp regexp("^.*((?:chatsend|chatrcv|msgsend|msgrcv|status|smssend),)");
			regexp.setMinimal(true);
			line.replace(regexp, "\\1");

			tokens = mySplit(',', line);
			if (tokens.count() < 2)
				continue;

			if (tokens.at(0) == "chatsend")
				entry.Type = HistoryEntry::ChatSend;
			else if (tokens.at(0) == "msgsend")
				entry.Type = HistoryEntry::MsgSend;
			else if (tokens.at(0) == "chatrcv")
				entry.Type = HistoryEntry::ChatRcv;
			else if (tokens.at(0) == "msgrcv")
				entry.Type = HistoryEntry::MsgRcv;
			else if (tokens.at(0) == "status")
				entry.Type = HistoryEntry::StatusChange;
			else if (tokens.at(0) == "smssend")
				entry.Type = HistoryEntry::SmsSend;

			switch (entry.Type)
			{
				case HistoryEntry::ChatSend:
				case HistoryEntry::MsgSend:
					if (tokens.count() == 5)
					{
						entry.Uin = tokens.at(1).toUInt();
						entry.Nick = tokens.at(2);
						entry.Date.setTime_t(tokens.at(3).toUInt());
						entry.Content = tokens.at(4);
						entries.append(entry);
					}
					break;
				case HistoryEntry::ChatRcv:
				case HistoryEntry::MsgRcv:
					if (tokens.count() == 6)
					{
						entry.Uin = tokens.at(1).toUInt();
						entry.Nick = tokens.at(2);
						entry.Date.setTime_t(tokens.at(3).toUInt());
						entry.SendDate.setTime_t(tokens.at(4).toUInt());
						entry.Content = tokens.at(5);
						entries.append(entry);
					}
					break;
				case HistoryEntry::StatusChange:
					if (tokens.count() == 6 || tokens.count() == 7)
					{
						entry.Uin = tokens.at(1).toUInt();
						entry.Nick = tokens.at(2);
						//entry.Ip = tokens.at(3);
						entry.Date.setTime_t(tokens.at(4).toUInt());
						if (tokens.at(5) == "avail")
							entry.Status = HistoryEntry::Online;
						else if (tokens.at(5) == "busy")
							entry.Status = HistoryEntry::Busy;
						else if (tokens.at(5) == "invisible")
							entry.Status = HistoryEntry::Invisible;
						else if (tokens.at(5) == "ffc")
							entry.Status = HistoryEntry::FFC;
						else if (tokens.at(5) == "dnd")
							entry.Status = HistoryEntry::DND;
						else if (tokens.at(5) == "notavail")
							entry.Status = HistoryEntry::Offline;
						if (tokens.count() == 7)
							entry.Content = tokens.at(6);
						entries.append(entry);
					}
					break;
				case HistoryEntry::SmsSend:
					if (tokens.count() == 4 || tokens.count() == 6)
					{
						entry.Mobile = tokens.at(1);
						entry.Date.setTime_t(tokens.at(2).toUInt());
						entry.Content = tokens.at(3);
						if (tokens.count() == 6)
						{
							entry.Nick = tokens.at(4);
							entry.Uin = tokens.at(5).toUInt();
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
		const int strlength = str.length();
		int idx = 0;
		bool inString = false;

		int pos1, pos2;
		while (idx < strlength)
		{
			const QChar letter = str.at(idx);
			if (inString)
			{
				if (letter == '\\')
				{
					switch (str.at(idx + 1).toAscii())
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
			else // out of string
			{
				if (letter == sep)
				{
					if (!token.isEmpty())
						token.clear();
					else
						strlist.append(QString());
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
}
