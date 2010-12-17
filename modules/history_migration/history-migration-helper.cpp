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
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QtAlgorithms>
#include <QtCore/QTextStream>

#include "misc/misc.h"
#include "misc/path-conversion.h"
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

	int getHistoryEntriesCount(const UinsList &uins)
	{
		kdebugf();

		int lines = 0;
		QString filename = getFileNameByUinsList(uins);
		QString path = profilePath("history/");
		QByteArray buffer;
		QFile fidx(path + filename + ".idx"), f(path + filename);

		if (fidx.open(QIODevice::ReadOnly))
		{
			int offs, lastOffs = 0;
			lines = fidx.size() / sizeof(int);

			// ignore garbage in index file (strange, but sometimes happens)
			while (fidx.read((char *)&offs, sizeof(int)) > 0)
			{
				if (offs < lastOffs)
					--lines;
				else
					lastOffs = offs;
			}
		}
		// apparentyly sms doesn't have an index file, so handle this
		else if (filename == "sms" && f.open(QIODevice::ReadOnly))
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

		fidx.close();

		kdebugmf(KDEBUG_INFO, "%d lines\n", lines);
		kdebugf2();
		return lines;
	}

	QList<UinsList> getUinsLists()
	{
		kdebugf();
		QList<UinsList> entries;
		QDir dir(profilePath("history/"), "*.idx");
		QStringList struins;
		UinsList uins;

		foreach (QString entry, dir.entryList())
		{
			uins.clear();
			// ignore sms.idx file, see below
			if (entry != "sms.idx")
			{
				struins = entry.remove(entry.length() - 4, 4).split('_', QString::SkipEmptyParts);
				foreach (const QString &struin, struins)
					uins.append(struin.toUInt());
				entries.append(uins);
			}
		}

		// special case for sms: probably it won't have an index file,
		// so only check if sms itself exists and append empty uins list
		if (QFile::exists(profilePath("history/sms")))
		{
			uins.clear();
			entries.append(uins);
		}

		kdebugf2();
		return entries;
	}

	QList<HistoryEntry> historyEntries(const UinsList &uins)
	{
		kdebugf();

		QList<HistoryEntry> entries;
		QStringList tokens;
		QFile f, fidx;
		QString path = profilePath("history/");
		QString filename, line;
		int offs = 0;

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
		if (fidx.open(QIODevice::ReadOnly))
			fidx.read((char *)&offs, sizeof(int));
		// let sms not have an index file
		else if (filename != "sms")
			return entries;
		fidx.close();
		if (!f.seek(offs))
			return entries;

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

			switch (entry.Type)
			{
				case HistoryEntry::ChatSend:
				case HistoryEntry::MsgSend:
					if (tokens.count() == 5)
					{
						entry.Uin = tokens[1].toUInt();
						entry.Nick = tokens[2];
						entry.Date.setTime_t(tokens[3].toUInt());
						entry.Content = tokens[4];
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
						entry.Content = tokens[5];
						entries.append(entry);
					}
					break;
				case HistoryEntry::StatusChange:
					if (tokens.count() == 6 || tokens.count() == 7)
					{
						entry.Uin = tokens[1].toUInt();
						entry.Nick = tokens[2];
						//entry.Ip = tokens[3];
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
							entry.Content = tokens[6];
						entries.append(entry);
					}
					break;
				case HistoryEntry::SmsSend:
					if (tokens.count() == 4 || tokens.count() == 6)
					{
						entry.Mobile = tokens[1];
						entry.Date.setTime_t(tokens[2].toUInt());
						entry.Content = tokens[3];
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
		int idx = 0, strlength = str.length();
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
};
