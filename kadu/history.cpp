/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <qwidget.h>
#include <qdialog.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qcstring.h>
#include <qfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qlayout.h>
#include <qfile.h>
#include <qbuffer.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qdir.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <time.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "kadu.h"
#include "config_dialog.h"
#include "misc.h"
#include "debug.h"
#include "history.h"

enum {
	HISTORYMANAGER_ORDINARY_LINE,
	HISTORYMANAGER_HISTORY_OUR,
	HISTORYMANAGER_HISTORY_FOREIGN,
	HISTORYMANAGER_SMS_WITH_NICK,
	HISTORYMANAGER_SMS_WITHOUT_NICK
};

QString HistoryManager::text2csv(const QString &text) {
	QString csv = text;
	csv.replace(QRegExp("\\\\"), "\\\\");
	csv.replace(QRegExp("\""), "\\\"");
	csv.replace(QRegExp("\r\n"), "\\n");
	csv.replace(QRegExp("\n"), "\\n");
	if (csv != text || text.find(QRegExp(","), 0) != -1)
		csv = QString("\"") + csv + QString("\"");	
	return csv;
}

QString HistoryManager::getFileNameByUinsList(UinsList &uins) {
	int i;
	QString fname;
	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));
		if (i < uins.count() - 1)
			fname.append("_");
		}
	return fname;
}

int HistoryManager::typeOfLine(const QString &line) {
	QStringList strlist;

	strlist = QStringList::split(" ", line);
	if (strlist.count() >= 6 && strlist[1].length() == 2 && strlist[1] == "::"
		&& strlist[2].length() == 2 && strlist[2][0].isDigit() && strlist[2][1].isDigit()
		&& strlist[3].length() == 2 && strlist[3][0].isDigit() && strlist[3][1].isDigit()
		&& strlist[4].length() == 5 && strlist[4][0].isDigit() && strlist[4][1].isDigit()
		&& strlist[4][2].isDigit() && strlist[4][3].isDigit() && strlist[4][4] == ','
		&& strlist[5].length() >= 9 && strlist[5][0] == '(' && strlist[5][1].isDigit()
		&& strlist[5][2].isDigit() && strlist[5][3] == ':' && strlist[5][4].isDigit()
		&& strlist[5][5].isDigit() && strlist[5][6] == ':' && strlist[5][7].isDigit()
		&& strlist[5][8].isDigit()) {
		if (strlist.count() == 6 && strlist[5].length() == 10 && strlist[5][9] == ')')
			return HISTORYMANAGER_HISTORY_OUR;
		if (strlist.count() == 9 && strlist[5].length() == 9 && strlist[6].length() == 1
			&& strlist[6] == "/" && strlist[7].length() == 1 && strlist[7] == "S"
			&& strlist[8].length() == 9 && strlist[8][0].isDigit() && strlist[8][1].isDigit()
			&& strlist[8][2] == ':' && strlist[8][3].isDigit() && strlist[8][4].isDigit()
			&& strlist[8][5] == ':' && strlist[8][6].isDigit() && strlist[8][7].isDigit()
			&& strlist[8][8] == ')')
			return HISTORYMANAGER_HISTORY_FOREIGN;
		}
	return HISTORYMANAGER_ORDINARY_LINE;
}

void HistoryManager::appendMessage(UinsList uins, uin_t uin, const QString &msg, bool own, time_t czas, bool chat) {
	QFile f, fidx;
	QString fname = ggPath("history/");
	QString line, nick;
	QStringList linelist;
	int i, offs;

	convHist2ekgForm(uins);
	fname.append(getFileNameByUinsList(uins));
		
	if (own)
		if (chat)
			linelist.append("chatsend");
		else
			linelist.append("msgsend");
	else
		if (chat)
			linelist.append("chatrcv");
		else
			linelist.append("msgrcv");
	linelist.append(QString::number(uin));
	if (userlist.containsUin(uin))
		nick = userlist.byUin(uin).altnick;
	else
		nick = QString::number(uin);
	linelist.append(text2csv(nick));
	linelist.append(QString::number(time(NULL)));
	if (!own)
		linelist.append(QString::number(czas));
	linelist.append(text2csv(msg));
	line = linelist.join(",");

	f.setName(fname);
	if (!(f.open(IO_WriteOnly | IO_Append))) {
		kdebug("HistoryManager::appendMessage(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}

	buildIndex(fname);
	fidx.setName(f.name() + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append)) {
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
		}

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	stream << line << '\n';

	f.close();
}

void HistoryManager::appendSms(const QString &mobile, const QString &msg)
{
	QFile f, fidx;
	QTextStream stream;
	QStringList linelist;
	QString altnick, line, fname;
	uin_t uin = 0;
	int offs;

	kdebug("HistoryManager::appendSms(): appending sms to history (%s)\n", mobile.local8Bit().data());
	
	convSms2ekgForm();

	linelist.append("smssend");
	linelist.append(mobile);
	linelist.append(QString::number(time(NULL)));
	linelist.append(text2csv(msg));

	for (int i = 0; i < userlist.count(); i++)
		if (userlist[i].mobile == mobile) {
			altnick = userlist[i].altnick;
			uin = userlist[i].uin;
			break;
			}
	if (uin) {
		UinsList uins;
		uins.append(uin);
		convHist2ekgForm(uins);
		linelist.append(text2csv(altnick));
		linelist.append(QString::number(uin));
		}

	line = linelist.join(",");

	f.setName(ggPath("history/sms"));
	if (!(f.open(IO_WriteOnly | IO_Append))) {
		kdebug("HistoryManager::appendSms(): Error opening sms history file\n");
		return;
		}

	buildIndex(f.name());
	fidx.setName(f.name() + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append)) {
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
		}

	stream.setDevice(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	stream << line << '\n';
	f.close();

	if (uin) {
		fname = ggPath("history/");
		fname = fname + QString::number(uin);
		f.setName(fname);
		if (!(f.open(IO_WriteOnly | IO_Append))) {
			kdebug("HistoryManager::appendSms(): Error opening sms history\n");
			return;
			}
		stream.setDevice(&f);
		stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
		stream << line << '\n';
		f.close();
		}
};

void HistoryManager::appendStatus(uin_t uin, unsigned int status, QString description) {
	QFile f, fidx;
	QString fname = ggPath("history/");
	QString line, nick, addr;
	QStringList linelist;
	int ip, offs;
	unsigned short port;
	struct in_addr in;

	kdebug("HistoryManager::appendStatus()\n");

	UinsList uins;
	uins.append(uin);
	convHist2ekgForm(uins);
	linelist.append("status");
	linelist.append(QString::number(uin));
	if (userlist.containsUin(uin)) {
		nick = userlist.byUin(uin).altnick;
		ip = userlist.byUin(uin).ip;
		port = userlist.byUin(uin).port;
		}
	else {
		nick = QString::number(uin);
		ip = 0;
		port = 0;
		}
	linelist.append(text2csv(nick));
	in.s_addr = ip;
	addr = inet_ntoa(in);
	if (port)
		addr = addr + QString(":") + QString::number(port);
	linelist.append(addr);
	linelist.append(QString::number(time(NULL)));
	switch (status) {
		case GG_STATUS_AVAIL:
		case GG_STATUS_AVAIL_DESCR:
			linelist.append("avail");
			break;
		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			linelist.append("busy");
			break;
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
		case GG_STATUS_INVISIBLE_DESCR:
			linelist.append("invisible");
			break;
		case GG_STATUS_NOT_AVAIL:
		case GG_STATUS_NOT_AVAIL_DESCR:
			linelist.append("notavail");
			break;
		}
	if (description != QString::null)
		linelist.append(text2csv(description));
	line = linelist.join(",");

	fname = fname + QString::number(uin);
	f.setName(fname);
	if (!(f.open(IO_WriteOnly | IO_Append))) {
		kdebug("HistoryManager::appendStatus(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}

	buildIndex(fname);
	fidx.setName(fname + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append)) {
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
		}

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	stream << line << '\n';

	f.close();
}

void HistoryManager::convHist2ekgForm(UinsList uins) {
	kdebug("HistoryManager::convHist2ekgForm()\n");

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QByteArray datain;
	QBuffer bufin, bufout;
	QStringList linelist;
	uin_t uin;
	int i, typeofline;

	fname = getFileNameByUinsList(uins);
	
	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite))) {
		kdebug("HistoryManager::convHist2ekgForm(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}
	datain = f.readAll();
	f.close();

	bufin.setBuffer(datain);
	bufin.open(IO_ReadOnly);

	bufout.open(IO_WriteOnly);

	QTextStream stream(&bufin);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	QTextStream streamout(&bufout);
	streamout.setCodec(QTextCodec::codecForName("ISO 8859-2"));

	bool our, foreign;
	QString dzien, miesiac, rok, czas, sczas, text, temp, lineout;
	QDateTime datetime, sdatetime;
	QRegExp sep("\\s");
	our = foreign = false;
	while ((line = stream.readLine()) != QString::null) {
		our = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		foreign = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
//		typeofline = typeOfLine(line);
//		our = (typeofline == HISTORYMANAGER_HISTORY_OUR);
//		foreign = (typeofline == HISTORYMANAGER_HISTORY_FOREIGN);
		if (our || foreign) {
			if (linelist.count()) {
				text.truncate(text.length() - 1);
				linelist.append(text2csv(text));
				lineout = linelist.join(",");
				streamout << lineout << '\n';
				}
			linelist.clear();
			text.truncate(0);
			nick = line.section(sep, 0, 0);
			dzien = line.section(sep, 2, 2);
			miesiac = line.section(sep, 3, 3);
			rok = line.section(sep, 4, 4);
			rok.truncate(rok.length() - 1);
			datetime.setDate(QDate(rok.toInt(), miesiac.toInt(), dzien.toInt()));
			sdatetime = datetime;
			czas = line.section(sep, 5, 5);
			czas.remove(0, 1);
			if (our) {
				czas.truncate(czas.length() - 1);
				linelist.append("chatsend");
				}
			datetime.setTime(QTime(czas.left(2).toInt(), czas.mid(3, 2).toInt(), czas.right(2).toInt()));
			if (foreign) {
				sczas = line.section(sep, 8, 8);
				sczas.truncate(sczas.length() - 1);
				sdatetime.setTime(QTime(sczas.left(2).toInt(), sczas.mid(3, 2).toInt(), sczas.right(2).toInt()));
				linelist.append("chatrcv");
				}
			if (our) {
				if (uins.count() > 1)
					uin = 0;
				else
					if (config.uin != uins[0])
						uin = uins[0];
					else
						uin = uins[1];
				}
			else
				if (userlist.containsAltNick(nick)) {
					UserListElement &user = userlist.byAltNick(nick);
					uin = user.uin;
					}
				else
					if (uins.count() > 1)
						uin = 0;
					else
						if (config.uin != uins[0])
							uin = uins[0];
						else
							uin = uins[1];
			linelist.append(QString::number(uin));
			if (our)
				if (userlist.containsUin(uin))
					nick = userlist.byUin(uin).altnick;
				else
					nick = QString::number(uin);
			linelist.append(nick);
			linelist.append(QString::number(-datetime.secsTo(
				QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
			if (foreign)
				linelist.append(QString::number(-sdatetime.secsTo(
					QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
			our = foreign = false;
			}
		else {
			if (!linelist.count())
				break;
			text.append(line);
			text.append("\n");
			}
		}
	if (linelist.count()) {
		text.truncate(text.length() - 1);
		linelist.append(text2csv(text));
		lineout = linelist.join(",");
		streamout << lineout << '\n';
		bufin.close();
		bufout.close();
		fnameout = fname + ".new";
		fout.setName(path + fnameout);
		if (!(fout.open(IO_WriteOnly | IO_Truncate))) {
			kdebug("HistoryManager::convHist2ekgForm(): Error opening new history file %s\n", (const char *)fnameout.local8Bit());
			return;
			}
		fout.writeBlock(bufout.buffer());
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		bufin.close();
		bufout.close();
		}
}

void HistoryManager::convSms2ekgForm() {
	kdebug("HistoryManager::convSms2ekgForm()\n");

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QStringList linelist;
	QBuffer bufin, bufout;
	QByteArray datain;
	uin_t uin;
	int i;

	fname = "sms";
	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite))) {
		kdebug("HistoryManager::convSms2ekgForm(): Error opening sms history file %s\n", (const char *)fname.local8Bit());
		return;
		}
	datain = f.readAll();
	f.close();

	bufin.setBuffer(datain);
	bufin.open(IO_ReadOnly);

	bufout.open(IO_WriteOnly);

	QTextStream stream(&bufin);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	QTextStream streamout(&bufout);
	streamout.setCodec(QTextCodec::codecForName("ISO 8859-2"));

	bool header;
	QString mobile, dzien, miesiac, rok, czas, text, temp, lineout;
	QDateTime datetime;
	QRegExp sep("\\s");
	header = false;
	while ((line = stream.readLine()) != QString::null) {
		header = !line.find(QRegExp("^\\S+\\s\\(\\d+\\)\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		if (header) {
			if (linelist.count()) {
				text.truncate(text.length() - 1);
				linelist.append(text2csv(text));
				if (uin) {
					linelist.append(nick);
					linelist.append(QString::number(uin));
					}
				lineout = linelist.join(",");
				streamout << lineout << '\n';
				}
			linelist.clear();
			text.truncate(0);
			nick = line.section(sep, 0, 0);
			uin = 0;
			mobile = line.section(sep, 1, 1);
			mobile.remove(0, 1);
			mobile.truncate(mobile.length() - 1);
			dzien = line.section(sep, 3, 3);
			miesiac = line.section(sep, 4, 4);
			rok = line.section(sep, 5, 5);
			rok.truncate(rok.length() - 1);
			datetime.setDate(QDate(rok.toInt(), miesiac.toInt(), dzien.toInt()));
			czas = line.section(sep, 6, 6);
			czas.remove(0, 1);
			czas.truncate(czas.length() - 1);
			linelist.append("smssend");
			linelist.append(mobile);
			datetime.setTime(QTime(czas.left(2).toInt(), czas.mid(3, 2).toInt(), czas.right(2).toInt()));
			linelist.append(QString::number(-datetime.secsTo(
				QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
			for (i = 0; i < userlist.count(); i++)
				if (userlist[i].mobile == mobile)
					uin = userlist[i].uin;
			header = false;
			}
		else {
			if (!linelist.count())
				break;
			text.append(line);
			text.append("\n");
			}
		}
	if (linelist.count()) {
		text.truncate(text.length() - 1);
		linelist.append(text2csv(text));
		if (uin) {
			linelist.append(nick);
			linelist.append(QString::number(uin));
			}
		lineout = linelist.join(",");
		streamout << lineout << '\n';
		bufin.close();
		bufout.close();
		fnameout = fname + ".new";
		fout.setName(path + fnameout);
		if (!(fout.open(IO_WriteOnly | IO_Truncate))) {
			kdebug("HistoryManager::convSms2EkgForm: Error opening new sms history file %s\n", (const char *)fnameout.local8Bit());
			return;
			}
		fout.writeBlock(bufout.buffer());
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		bufin.close();
		bufout.close();
		}
}

int HistoryManager::getHistoryEntriesCountPrivate(const QString &filename) {
	kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename)\n");

	int lines;
	QFile f;
	QString path = ggPath("history/");
	QByteArray buffer;

	f.setName(path + filename);
	if (!(f.open(IO_ReadOnly))) {
		kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename): Error opening history file %s\n", (const char *)filename.local8Bit());
		return -1;
		}
	buffer = f.readAll();
	f.close();
	lines = buffer.contains('\n');
	kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename): %d lines\n", lines);
	return lines;
}

int HistoryManager::getHistoryEntriesCount(UinsList uins) {
	return getHistoryEntriesCountPrivate(getFileNameByUinsList(uins));
}

int HistoryManager::getHistoryEntriesCount(QString mobile) {
	if (mobile == QString::null)
		return getHistoryEntriesCountPrivate("sms");
	else
		return getHistoryEntriesCountPrivate(mobile);
}

QValueList<HistoryEntry> HistoryManager::getHistoryEntries(UinsList uins, int from, int count, int mask) {
	kdebug("HistoryManager::getHistoryEntries(UinsList uins, int from, int count)\n");

	QValueList<HistoryEntry> entries;
	QStringList tokens;
	QFile f, fidx;
	QString path = ggPath("history/");
	QString filename, line;
	int offs;

	filename = getFileNameByUinsList(uins);
	f.setName(path + filename);
	if (!(f.open(IO_ReadOnly))) {
		kdebug("HistoryManager::getHistoryEntries(UinsList uins, int from, int count): Error opening history file %s\n", (const char *)filename.local8Bit());
		return entries;
		}

	fidx.setName(f.name() + ".idx");
	if (!fidx.open(IO_ReadOnly))
		return entries;
	fidx.at(from * sizeof(int));
	fidx.readBlock((char *)&offs, (Q_LONG)sizeof(int));
	fidx.close();
	if (!f.at(offs))
		return entries;

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	
	int linenr = from;

	struct HistoryEntry entry;
	while (linenr < from + count && (line = stream.readLine()) != QString::null) {
		linenr++;
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
		switch (entry.type) {
			case HISTORYMANAGER_ENTRY_CHATSEND:
			case HISTORYMANAGER_ENTRY_MSGSEND:
				if (tokens.count() == 5) {
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
				if (tokens.count() == 6) {
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
				if (tokens.count() == 6 || tokens.count() == 7) {
					entry.uin = tokens[1].toUInt();
					entry.nick = tokens[2];
					entry.ip = tokens[3];
					entry.date.setTime_t(tokens[4].toUInt());
					if (tokens[5] == "avail")
						entry.status = GG_STATUS_AVAIL;
					else if (tokens[5] == "notavail")
						entry.status = GG_STATUS_NOT_AVAIL;
					else if (tokens[5] == "busy")
						entry.status = GG_STATUS_BUSY;
					else if (tokens[5] == "invisible")
						entry.status = GG_STATUS_INVISIBLE;
					if (tokens.count() == 7) {
						switch (entry.status) {
							case GG_STATUS_AVAIL:
								entry.status = GG_STATUS_AVAIL_DESCR;
								break;
							case GG_STATUS_NOT_AVAIL:
								entry.status = GG_STATUS_NOT_AVAIL_DESCR;
								break;
							case GG_STATUS_BUSY:
								entry.status = GG_STATUS_BUSY_DESCR;
								break;
							case GG_STATUS_INVISIBLE:
								entry.status = GG_STATUS_INVISIBLE_DESCR;
								break;
							}
						entry.description = tokens[6];
						}
					else
						entry.description.truncate(0);
					entry.mobile.truncate(0);
					entry.message.truncate(0);
					entries.append(entry);
					}
				break;
			case HISTORYMANAGER_ENTRY_SMSSEND:
				if (tokens.count() == 4 || tokens.count() == 6) {
					entry.mobile = tokens[1];
					entry.date.setTime_t(tokens[2].toUInt());
					entry.message = tokens[3];
					if (tokens.count() == 4) {
						entry.nick.truncate(0);
						entry.uin = 0;
						}
					else {
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

	return entries;
}

void HistoryManager::buildIndex(const QString &filename) {
	kdebug("HistoryManager::buildIndex()\n");
	QString fnameout = filename + ".idx";
	char *inbuf;
	int *outbuf;
	int inbufoffs, outbufoffs, inoffs;
	Q_LONG read, written;

	if (QFile::exists(fnameout))
		return;
	QFile fin(filename);
	QFile fout(fnameout);
	if (!fin.open(IO_ReadOnly)) {
		kdebug("HistoryManager::buildIndex(): Error opening history file: %s\n", (const char *)fin.name().local8Bit());
		return;
		}
	if (!fout.open(IO_WriteOnly | IO_Truncate)) {
		kdebug("HistoryManager::buildIndex(): Error creating history index file: %s\n", (const char *)fout.name().local8Bit());
		fin.close();
		return;
		}
	inbuf = new char[65536];
	outbuf = new int[4096];

	inoffs = outbufoffs = 0;
	while ((read = fin.readBlock(inbuf, 65536)) > 0) {
		inbufoffs = 0;
		while (inbufoffs < read) {
			outbuf[outbufoffs++] = inoffs + inbufoffs;
			if (outbufoffs == 4096) {
				written = fout.writeBlock((char *)outbuf, 4096 * sizeof(int));
				outbufoffs = 0;
				}
			while (inbufoffs < read && inbuf[inbufoffs] != '\n')
				inbufoffs++;
			if (inbufoffs < read) {
				inbufoffs++;
				if (inbufoffs == read)
					inoffs += read;
				}
			}
		}
	if (outbufoffs)
		written = fout.writeBlock((char *)outbuf, outbufoffs * sizeof(int));

	delete inbuf;
	delete outbuf;

	fin.close();
	fout.close();
}

QStringList HistoryManager::mySplit(const QChar &sep, const QString &str) {
	QStringList strlist;

	QString token;
	QChar letter;
	int idx = 0, state = 0;
	while (idx < str.length()) {
		letter = str[idx];
		switch (state) {
			case 0:
				if (letter == ',') {
					if (token.length())
						token.truncate(0);
					else
						strlist.append(token);
					}
				else
					if (letter == '"')
						state = 2;
					else {
						token.append(letter);
						state = 1;
						}
				idx++;
				break;
			case 1:
				if (letter != ',') {
					token.append(letter);
					idx++;
					}
				else {
					strlist.append(token);
					state = 0;
					}
				break;
			case 2:
				if (letter == '\\')
					state = 3;
				else
					if (letter == '\"') {
						strlist.append(token);
						state = 0;
						}
					else
						token.append(letter);
				idx++;
				break;
			case 3:
				switch (letter) {
					case 'n':
						token.append('\n');
						break;
					case '\\':
						token.append('\\');
						break;
					case '\"':
						token.append('\"');
						break;
					default:
						token.append('?');
					}
				state = 2;
				idx++;
				break;
			}
		}
	if (state == 1)
		strlist.append(token);

	return strlist;
}

History::History(UinsList uins) {
	int i;
	
	history.convHist2ekgForm(uins);

	setCaption(i18n("History"));
	setWFlags(Qt::WDestructiveClose);

	QGridLayout *grid = new QGridLayout(this, 2,1,3,3);

	body = new QMultiLineEdit(this, "History browser");
	body->setReadOnly(true);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setWrapPolicy(QMultiLineEdit::Anywhere);
	QPushButton *closebtn = new QPushButton(this);
	closebtn->setText(i18n("&Close"));

	QString fname;
	fname.append(ggPath("history/"));
	fname.append(HistoryManager::getFileNameByUinsList(uins));
		
	grid->addWidget(body,0,0);
	grid->addWidget(closebtn,1,0, Qt::AlignRight);

	connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	resize(500,400);

	int count;
	QString text;

	kdebug("History(): lines = %d\n", count = history.getHistoryEntriesCount(uins));
	QValueList<HistoryEntry> entries;
	entries = history.getHistoryEntries(uins, 0, count);
	for (i = 0; i < entries.count(); i++) {
		switch (entries[i].type) {
			case HISTORYMANAGER_ENTRY_CHATSEND:
			case HISTORYMANAGER_ENTRY_MSGSEND:
				text.append(config.nick);
				text.append(entries[i].date.toString(" (dd.MM.yyyy hh:mm:ss)\n"));
				text.append(entries[i].message + "\n\n");
				break;
			case HISTORYMANAGER_ENTRY_CHATRCV:
			case HISTORYMANAGER_ENTRY_MSGRCV:
				text.append(entries[i].nick);
				text.append(entries[i].date.toString(" (dd.MM.yyyy hh:mm:ss / S "));
				text.append(entries[i].sdate.toString("dd.MM.yyyy hh:mm:ss)\n"));
				text.append(entries[i].message + "\n\n");
				break;
			case HISTORYMANAGER_ENTRY_STATUS:
				text.append(entries[i].nick);
				text.append(entries[i].date.toString(" (dd.MM.yyyy hh:mm:ss) ip="));
				text.append(entries[i].ip + "\n");
				switch (entries[i].status) {
					case GG_STATUS_AVAIL:
					case GG_STATUS_AVAIL_DESCR:
						text.append(i18n("Online"));
						break;
					case GG_STATUS_BUSY:
					case GG_STATUS_BUSY_DESCR:
						text.append(i18n("Busy"));
						break;
					case GG_STATUS_INVISIBLE:
					case GG_STATUS_INVISIBLE_DESCR:
						text.append(i18n("Invisible"));
						break;
					case GG_STATUS_NOT_AVAIL:
					case GG_STATUS_NOT_AVAIL_DESCR:
						text.append(i18n("Offline"));
						break;
					}
				if (entries[i].description.length())
					text.append(QString(" (") + entries[i].description + ")\n\n");
				else
					text.append("\n\n");
				break;
			case HISTORYMANAGER_ENTRY_SMSSEND:
				text.append(entries[i].mobile + " SMS");
				text.append(entries[i].date.toString(" (dd.MM.yyyy hh:mm:ss)\n"));
				text.append(entries[i].message + "\n\n");
				break;
			}
		}
	body->setText(text);
}

HistoryManager history;
