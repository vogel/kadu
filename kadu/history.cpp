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
#include <qtextbrowser.h>
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
#include <qtextcodec.h>
#include <qregexp.h>
#include <qdir.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qhgroupbox.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qevent.h>

#include <time.h>
//#include <sys/socket.h>
//#include <net/if.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

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

	buildIndexPrivate(fname);
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

	buildIndexPrivate(f.name());
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
		}
};

void HistoryManager::appendStatus(uin_t uin, unsigned int status, QString description) {
	QFile f, fidx;
	QString fname = ggPath("history/");
	QString line, nick, addr;
	QStringList linelist;
	int offs;
	QHostAddress ip;
	unsigned short port;
//	struct in_addr in;

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
		ip.setAddress((unsigned int)0);
		port = 0;
		}
	linelist.append(text2csv(nick));
	addr = ip.toString();
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

	buildIndexPrivate(fname);
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

void HistoryManager::removeHistory(UinsList uins) {
	kdebug("HistoryManager::removeHistory()\n");

	QString fname;
	switch (QMessageBox::information(kadu, "Kadu", i18n("Clear history?"),
		i18n("Yes"), i18n("No"), QString::null, 1, 1)) {
		case 0:
			fname = ggPath("history/");
			fname.append(getFileNameByUinsList(uins));
			kdebug("HistoryManager::removeHistory(): deleting %s\n", (const char *)fname.local8Bit());
			QFile::remove(fname);
			QFile::remove(fname + ".idx");
			break;
		case 1:
			break;
		}
}

void HistoryManager::convHist2ekgForm(UinsList uins) {
	kdebug("HistoryManager::convHist2ekgForm()\n");

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QStringList linelist;
	uin_t uin;
	int i, j, typeofline;

	fname = getFileNameByUinsList(uins);
	
	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite))) {
		kdebug("HistoryManager::convHist2ekgForm(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}

	fnameout = fname + ".new";
	fout.setName(path + fnameout);
	if (!(fout.open(IO_WriteOnly | IO_Truncate))) {
		kdebug("HistoryManager::convHist2ekgForm(): Error opening new history file %s\n", (const char *)fnameout.local8Bit());
		f.close();
		return;
		}

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	QTextStream streamout(&fout);
	streamout.setCodec(QTextCodec::codecForName("ISO 8859-2"));

	bool our, foreign;
	QString dzien, miesiac, rok, czas, sczas, text, temp, lineout;
	QDateTime datetime, sdatetime;
	QRegExp sep("\\s"), sep2("::");
	our = foreign = false;
	while ((line = stream.readLine()) != QString::null) {
//		our = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		our = !line.find(QRegExp("^(\\S|\\s)+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
//		foreign = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		foreign = !line.find(QRegExp("^(\\S|\\s)+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		if (our || foreign) {
			if (linelist.count()) {
				text.truncate(text.length() - 1);
				if (text[text.length() - 1] == '\n')
					text.truncate(text.length() - 1);
				linelist.append(text2csv(text));
				lineout = linelist.join(",");
				streamout << lineout << '\n';
				}
			linelist.clear();
			text.truncate(0);
			nick = line.section(sep2, 0, 0);
			nick.truncate(nick.length() - 1);
			line = line.right(line.length() - nick.length() - 4);
			dzien = line.section(sep, 0, 0);
			miesiac = line.section(sep, 1, 1);
			rok = line.section(sep, 2, 2);
			rok.truncate(rok.length() - 1);
			datetime.setDate(QDate(rok.toInt(), miesiac.toInt(), dzien.toInt()));
			sdatetime = datetime;
			czas = line.section(sep, 3, 3);
			czas.remove(0, 1);
			if (our) {
				czas.truncate(czas.length() - 1);
				linelist.append("chatsend");
				}
			datetime.setTime(QTime(czas.left(2).toInt(), czas.mid(3, 2).toInt(), czas.right(2).toInt()));
			if (foreign) {
				sczas = line.section(sep, 6, 6);
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
		if (text[text.length() - 1] == '\n')
			text.truncate(text.length() - 1);
		linelist.append(text2csv(text));
		lineout = linelist.join(",");
		streamout << lineout << '\n';
		f.close();
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		f.close();
		fout.remove();
		}
}

void HistoryManager::convSms2ekgForm() {
	kdebug("HistoryManager::convSms2ekgForm()\n");

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QStringList linelist;
	uin_t uin;
	int i;

	fname = "sms";
	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite))) {
		kdebug("HistoryManager::convSms2ekgForm(): Error opening sms history file %s\n", (const char *)fname.local8Bit());
		return;
		}
	fnameout = fname + ".new";
	fout.setName(path + fnameout);
	if (!(fout.open(IO_WriteOnly | IO_Truncate))) {
		kdebug("HistoryManager::convSms2EkgForm: Error opening new sms history file %s\n", (const char *)fnameout.local8Bit());
		f.close();
		return;
		}

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	QTextStream streamout(&fout);
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
				if (text[text.length() - 1] == '\n')
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
		if (text[text.length() - 1] == '\n')
			text.truncate(text.length() - 1);
		linelist.append(text2csv(text));
		if (uin) {
			linelist.append(nick);
			linelist.append(QString::number(uin));
			}
		lineout = linelist.join(",");
		streamout << lineout << '\n';
		f.close();
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		f.close();
		fout.remove();
		}
}

int HistoryManager::getHistoryEntriesCountPrivate(const QString &filename) {
	kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename)\n");

	int lines;
	QFile f;
	QString path = ggPath("history/");
	QByteArray buffer;

	f.setName(path + filename + ".idx");
	if (!(f.open(IO_ReadOnly))) {
		kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename): Error opening history file %s\n", (const char *)filename.local8Bit());
		return -1;
		}
	lines = f.size() / sizeof(int);
//	buffer = f.readAll();
	f.close();
//	lines = buffer.contains('\n');
	
	kdebug("HistoryManager::getHistoryEntriesCountPrivate(const QString &filename): %d lines\n", lines);
	return lines;
}

int HistoryManager::getHistoryEntriesCount(UinsList uins) {
	convHist2ekgForm(uins);
	buildIndex(uins);
	return getHistoryEntriesCountPrivate(getFileNameByUinsList(uins));
}

int HistoryManager::getHistoryEntriesCount(QString mobile) {
	convSms2ekgForm();
	buildIndex();
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

void HistoryManager::buildIndexPrivate(const QString &filename) {
	kdebug("HistoryManager::buildIndexPrivate()\n");
	QString fnameout = filename + ".idx";
	char *inbuf;
	int *outbuf;
	int inbufoffs, outbufoffs, inoffs;
	Q_LONG read, written;
	bool saved = false;

	if (QFile::exists(fnameout))
		return;
	QFile fin(filename);
	QFile fout(fnameout);
	if (!fin.open(IO_ReadOnly)) {
		kdebug("HistoryManager::buildIndexPrivate(): Error opening history file: %s\n", (const char *)fin.name().local8Bit());
		return;
		}
	if (!fout.open(IO_WriteOnly | IO_Truncate)) {
		kdebug("HistoryManager::buildIndexPrivate(): Error creating history index file: %s\n", (const char *)fout.name().local8Bit());
		fin.close();
		return;
		}
	inbuf = new char[65536];
	outbuf = new int[4096];

	inoffs = outbufoffs = 0;
	while ((read = fin.readBlock(inbuf, 65536)) > 0) {
		inbufoffs = 0;
		while (inbufoffs < read) {
			if (saved)
				saved = false;
			else
				outbuf[outbufoffs++] = inoffs + inbufoffs;
			if (outbufoffs == 4096) {
				written = fout.writeBlock((char *)outbuf, 4096 * sizeof(int));
				outbufoffs = 0;
				}
			while (inbufoffs < read && inbuf[inbufoffs] != '\n')
				inbufoffs++;
			if (inbufoffs < read)
				inbufoffs++;
			if (inbufoffs == read) {
				inoffs += read;
				saved = true;
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

void HistoryManager::buildIndex(UinsList uins) {
	buildIndexPrivate(ggPath("history/") + getFileNameByUinsList(uins));
}

void HistoryManager::buildIndex(QString mobile) {
	if (mobile == QString::null)
		buildIndexPrivate(ggPath("history/") + "sms");
	else
		buildIndexPrivate(ggPath("history/") + mobile);
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

int HistoryManager::getHistoryEntryIndexByDate(UinsList uins, QDateTime &date, bool enddate)
{
	kdebug("HistoryManager::getHistoryEntryIndexByDate()\n");

	QValueList<HistoryEntry> entries;
	int count = getHistoryEntriesCount(uins);
	int start, end;
	
	start = 0;
	end = count - 1;
	while (end - start >= 0) {
		kdebug("HistoryManager::getHistoryEntryIndexByDate(): start = %d, end = %d\n", start, end);
		entries = getHistoryEntries(uins, start + ((end - start) / 2), 1);
		if (entries.count())
			if (date < entries[0].date)
				end -= ((end - start) / 2) + 1;
			else
				if (date > entries[0].date)
					start += ((end - start) / 2) + 1;
				else
					return start + ((end - start) / 2);
		}
	if (end < 0) {
		kdebug("HistoryManager::getHistoryEntryIndexByDate(): return minus one\n");
		return -1;
		}
	if (start >= count) {
		kdebug("HistoryManager::getHistoryEntryIndexByDate(): return count\n");
		return count;
		}
	if (enddate) {
		entries = getHistoryEntries(uins, start, 1);
		if (entries.count() && date < entries[0].date)
			start--;
		}
	kdebug("HistoryManager::getHistoryEntryIndexByDate(): return %d\n", start);
	return start;
}

History::History(UinsList uins): uins(uins), closeDemand(false), finding(false) {
	int i;
	
	history.convHist2ekgForm(uins);
	history.buildIndex(uins);

	setCaption(i18n("History"));
	setWFlags(Qt::WDestructiveClose);

	QGridLayout *grid = new QGridLayout(this, 2, 4, 3, 3);

	body = new QTextBrowser(this, "History browser");
	body->setReadOnly(true);
	body->setFont(config.fonts.chat);

//	QPushButton *closebtn = new QPushButton(this);
//	closebtn->setText(i18n("&Close"));
	QPushButton *prevbtn = new QPushButton(this);
	prevbtn->setPixmap(loadIcon("back.png"));
	QPushButton *nextbtn = new QPushButton(this);
	nextbtn->setPixmap(loadIcon("forward.png"));
	QPushButton *searchbtn = new QPushButton(this);
	searchbtn->setText(i18n("&Find"));
	QPushButton *searchnextbtn = new QPushButton(this);
	searchnextbtn->setText(i18n("Find &next"));
	QPushButton *searchprevbtn = new QPushButton(this);
	searchprevbtn->setText(i18n("Find &previous"));

	grid->addMultiCellWidget(body, 0, 0, 0, 4);
	grid->addWidget(prevbtn, 1, 0);
	grid->addWidget(nextbtn, 1, 1);
	grid->addWidget(searchbtn, 1, 2);
	grid->addWidget(searchnextbtn, 1, 3);
	grid->addWidget(searchprevbtn, 1, 4);
//	grid->addWidget(closebtn, 1, 5, Qt::AlignRight);

	connect(prevbtn, SIGNAL(clicked()), this, SLOT(prevBtnClicked()));
	connect(nextbtn, SIGNAL(clicked()), this, SLOT(nextBtnClicked()));
	connect(searchbtn, SIGNAL(clicked()), this, SLOT(searchBtnClicked()));
	connect(searchnextbtn, SIGNAL(clicked()), this, SLOT(searchNextBtnClicked()));
	connect(searchprevbtn, SIGNAL(clicked()), this, SLOT(searchPrevBtnClicked()));
//	connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	resize(500,400);

	findrec.type = 1;
	findrec.reverse = 0;
	findrec.actualrecord = -1;

	int count = history.getHistoryEntriesCount(uins);
	start = count - 100 < 0 ? 0 : count - 100;
	count -= start;
	showHistoryEntries(start, count);
}

void History::prevBtnClicked() {
	int count = history.getHistoryEntriesCount(uins) - start;
	start -= 100;
	if (start < 0)
		start = 0;
	if (count > 100)
		count = 100;
	showHistoryEntries(start, count);
}

void History::nextBtnClicked() {
	int count = history.getHistoryEntriesCount(uins);
	start += 100;
	if (start > count - 100)
		start = count - 100;
	if (start < 0)
		start = 0;
	count = count - start > 100 ? 100 : count - start;
	showHistoryEntries(start, count);
}

void History::formatHistoryEntry(QString &text, const HistoryEntry &entry) {
	QString bgcolor, textcolor;
	QString message;

	message = entry.message;
	message.replace(QRegExp("\n"), "<BR>");

	text.append("<TABLE width=\"100%\"><TR><TD bgcolor=\"");
	if (entry.type & (HISTORYMANAGER_ENTRY_CHATSEND | HISTORYMANAGER_ENTRY_MSGSEND
		| HISTORYMANAGER_ENTRY_SMSSEND)) {
		bgcolor = config.colors.mychatBg.name();
		textcolor = config.colors.mychatText.name();
		}
	else {
		bgcolor = config.colors.usrchatBg.name();
		textcolor = config.colors.usrchatText.name();
		}
	text.append(bgcolor);
	text.append("\"><FONT color=\"");
	text.append(textcolor);
	text.append("\"><B>");
	if (entry.type == HISTORYMANAGER_ENTRY_SMSSEND)
		text.append(entry.mobile + " SMS");
	else
		if (entry.type & (HISTORYMANAGER_ENTRY_CHATSEND | HISTORYMANAGER_ENTRY_MSGSEND))
			text.append(config.nick);
		else
			text.append(entry.nick);
	text.append(QString(" :: ") + printDateTime(entry.date));
	if (entry.type & (HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV))
		text.append(QString(" / S ") + printDateTime(entry.sdate));
	text.append("</B><BR>");
	if (entry.type & HISTORYMANAGER_ENTRY_STATUS) {
		switch (entry.status) {
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
		if (entry.description.length())
			text.append(QString(" (") + entry.description + ")");
		text.append(QString(" ip=") + entry.ip);
		}
	else
		text.append(message);
	text.append("</TD></TR></TABLE></FONT>");
}

void History::showHistoryEntries(int from, int count) {
	int i;
	QString text;

	QValueList<HistoryEntry> entries;
	entries = history.getHistoryEntries(uins, from, count);
	for (i = 0; i < entries.count(); i++)
		formatHistoryEntry(text, entries[i]);
	body->setText(text);
}

void History::searchBtnClicked() {
	kdebug("History::searchBtnClicked()\n");

	HistorySearch *hs;
	hs = new HistorySearch(this, uins);
//	hs->resetBtnClicked();
	hs->setDialogValues(findrec);
	if (hs->exec() == QDialog::Accepted) {
		findrec = hs->getDialogValues();
		findrec.actualrecord = -1;
		searchHistory();
		}
	delete hs;
}

void History::searchNextBtnClicked() {
	kdebug("History::searchNextBtnClicked()\n");
	findrec.reverse = false;
	searchHistory();
}

void History::searchPrevBtnClicked() {
	kdebug("History::searchPrevBtnClicked()\n");
	findrec.reverse = true;
	searchHistory();
}

QString History::gaduStatus2symbol(unsigned int status) {
	switch (status) {
		case GG_STATUS_AVAIL:
		case GG_STATUS_AVAIL_DESCR:
			return QString("avail");
		case GG_STATUS_BUSY:
		case GG_STATUS_BUSY_DESCR:
			return QString("busy");
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE_DESCR:
			return QString("invisible");
		default:
			return QString("notavail");
		}
}

void History::searchHistory() {
	kdebug("History::searchHistory()\n");
	int start, end, count, total, i, len;
	QDateTime fromdate, todate;
	QValueList<HistoryEntry> entries;
	QRegExp rxp;

	count = history.getHistoryEntriesCount(uins);
	if (findrec.fromdate.isNull())
		start = 0;
	else
		start = history.getHistoryEntryIndexByDate(uins, findrec.fromdate);
	if (findrec.todate.isNull())
		end = count - 1;
	else
		end = history.getHistoryEntryIndexByDate(uins, findrec.todate, true);
	kdebug("History::searchHistory(): start = %d, end = %d\n", start, end);
	if (start > end || (start == end && (start == -1 || start == count)))
		return;
	if (start == -1)
		start = 0;
	if (end == count)
		end--;
	entries = history.getHistoryEntries(uins, start, 1);
	fromdate = entries[0].date;
	entries = history.getHistoryEntries(uins, end, 1);
	todate = entries[0].date;	
	kdebug("History::searchHistory(): start = %s, end = %s\n",
		fromdate.toString("dd.MM.yyyy hh:mm:ss").latin1(),
		todate.toString("dd.MM.yyyy hh:mm:ss").latin1());
	if (findrec.actualrecord == -1)
		findrec.actualrecord = findrec.reverse ? end : start;
	if ((findrec.actualrecord >= end && !findrec.reverse)
		|| (findrec.actualrecord <= start && findrec.reverse))
		return;
	if (findrec.reverse)
		total = findrec.actualrecord - start + 1;
	else
		total = end - findrec.actualrecord + 1;
	kdebug("History::searchHistory(): findrec.type = %d\n", findrec.type);
	rxp.setPattern(findrec.data);
	setEnabled(false);
	finding = true;
	if (findrec.reverse)
		do {
			len = total > 100 ? 100 : total;
			entries = history.getHistoryEntries(uins, findrec.actualrecord - len + 1, len);
			for (i = 0; i < entries.count(); i++)
				if ((findrec.type == 1 &&
					(entries[entries.count() - i - 1].type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& entries[entries.count() - i - 1].message.contains(rxp)) ||
					(findrec.type == 2 &&
					(entries[entries.count() - i - 1].type & HISTORYMANAGER_ENTRY_STATUS)
					&& findrec.data == gaduStatus2symbol(entries[entries.count() - i - 1].status))) {
					kdebug("History::searchHistory(): showHistoryEntries()\n");
					showHistoryEntries(findrec.actualrecord - i,
						findrec.actualrecord - i + 99 < count ? 100
						: count - findrec.actualrecord + i);
					History::start = findrec.actualrecord - i;
					break;
					}
			findrec.actualrecord -= i + (i < entries.count());
			total -= i + (i < entries.count());
			kdebug("History::searchHistory(): actualrecord = %d, i = %d, total = %d\n",
				findrec.actualrecord, i, total);
			a->processEvents();
		} while (total > 0 && i == entries.count() && !closeDemand);
	else
		do {
			len = total > 100 ? 100 : total;
			entries = history.getHistoryEntries(uins, findrec.actualrecord, len);
			for (i = 0; i < entries.count(); i++)
				if ((findrec.type == 1 && (entries[i].type & HISTORYMANAGER_ENTRY_ALL_MSGS)
					&& entries[i].message.contains(rxp)) ||
					(findrec.type == 2 &&
					(entries[i].type & HISTORYMANAGER_ENTRY_STATUS) &&
					findrec.data == gaduStatus2symbol(entries[i].status))) {
					kdebug("History::searchHistory(): showHistoryEntries()\n");
					showHistoryEntries(findrec.actualrecord + i,
						findrec.actualrecord + 99 < count ? 100
						: count - findrec.actualrecord - i);
					History::start = findrec.actualrecord + i;
					break;
					}
			findrec.actualrecord += i + (i < entries.count());
			total -= i + (i < entries.count());
			kdebug("History::searchHistory(): actualrecord = %d, i = %d, total = %d\n",
				findrec.actualrecord, i, total);
			a->processEvents();
		} while (total > 0 && i == entries.count() && !closeDemand);
	if (closeDemand) {
		reject();
		return;
		}
	if (findrec.actualrecord < 0)
		findrec.actualrecord = 0;
	setEnabled(true);
	finding = false;
}

void History::closeEvent(QCloseEvent *e) {
	if (finding) {
		e->ignore();
		closeDemand = true;
		}
	else
		e->accept();
}

HistorySearch::HistorySearch(QWidget *parent, UinsList uins) : QDialog(parent), uins(uins) {
	setCaption(i18n("Search history"));

	int i;
	char buf[128];

	for (i = 0; i <= 59; i++) {
		sprintf(buf, "%02d", i);
		numslist.append(QString(buf));
		}
	
	QStringList yearslist;
	for (i = 2000; i <= 2020; i++)
		yearslist.append(QString::number(i));
	QStringList dayslist;
	for (i = 1; i <= 31; i++)
		dayslist.append(numslist[i]);
	QStringList monthslist;
	for (i = 1; i <= 12; i++)
		monthslist.append(numslist[i]);
	QStringList hourslist;
	for (i = 0; i <= 23; i++)
		hourslist.append(numslist[i]);
	QStringList minslist;
	for (i = 0; i <= 59; i++)
		minslist.append(numslist[i]);
	
	QHBox *from_hb = new QHBox(this);
	from_chb = new QCheckBox(i18n("&From:") ,from_hb);
	from_hgb = new QHGroupBox(from_hb);
	from_day_cob = new QComboBox(from_hgb);
	from_day_cob->insertStringList(dayslist);
	QToolTip::add(from_day_cob, i18n("day"));
	from_month_cob = new QComboBox(from_hgb);
	from_month_cob->insertStringList(monthslist);
	QToolTip::add(from_month_cob, i18n("month"));
	from_year_cob = new QComboBox(from_hgb);
	from_year_cob->insertStringList(yearslist);
	QToolTip::add(from_year_cob, i18n("year"));
	from_hour_cob = new QComboBox(from_hgb);
	from_hour_cob->insertStringList(hourslist);
	QToolTip::add(from_hour_cob, i18n("hour"));
	from_min_cob = new QComboBox(from_hgb);
	from_min_cob->insertStringList(minslist);
	QToolTip::add(from_min_cob, i18n("minute"));

	QHBox *to_hb = new QHBox(this);
	to_chb = new QCheckBox(i18n("&To:") ,to_hb);
	to_hgb = new QHGroupBox(to_hb);
	to_day_cob = new QComboBox(to_hgb);
	to_day_cob->insertStringList(dayslist);
	QToolTip::add(to_day_cob, i18n("day"));
	to_month_cob = new QComboBox(to_hgb);
	to_month_cob->insertStringList(monthslist);
	QToolTip::add(to_month_cob, i18n("month"));
	to_year_cob = new QComboBox(to_hgb);
	to_year_cob->insertStringList(yearslist);
	QToolTip::add(to_year_cob, i18n("year"));
	to_hour_cob = new QComboBox(to_hgb);
	to_hour_cob->insertStringList(hourslist);
	QToolTip::add(to_hour_cob, i18n("hour"));
	to_min_cob = new QComboBox(to_hgb);
	to_min_cob->insertStringList(minslist);
	QToolTip::add(to_min_cob, i18n("minute"));

	criteria_bg = new QVButtonGroup(i18n("Find Criteria"), this);
	phrase_rb = new QRadioButton(i18n("&Pattern"), criteria_bg);
	status_rb = new QRadioButton(i18n("&Status"), criteria_bg);
	criteria_bg->insert(phrase_rb, 1);
	criteria_bg->insert(status_rb, 2);

	phrase_hgb = new QHGroupBox(i18n("Pattern"), this);
	phrase_edit = new QLineEdit(phrase_hgb);
	status_hgb = new QHGroupBox(i18n("Status"), this);
	status_cob = new QComboBox(status_hgb);
	for (i = 0; i < 4; i++)
		status_cob->insertItem(i18n(statustext[i * 2]));

	reverse_chb = new QCheckBox(i18n("&Reverse find"), this);

	QPushButton *find_btn = new QPushButton(i18n("&Find"), this);
	QPushButton *reset_btn = new QPushButton(i18n("Reset"), this);
	QPushButton *cancel_btn = new QPushButton(i18n("&Cancel"), this);

	connect(from_chb, SIGNAL(toggled(bool)), this, SLOT(fromToggled(bool)));
	connect(from_month_cob, SIGNAL(activated(int)), this, SLOT(correctFromDays(int)));
	connect(to_chb, SIGNAL(toggled(bool)), this, SLOT(toToggled(bool)));
	connect(to_month_cob, SIGNAL(activated(int)), this, SLOT(correctToDays(int)));
	connect(criteria_bg, SIGNAL(clicked(int)), this, SLOT(criteriaChanged(int)));
	connect(find_btn, SIGNAL(clicked()), this, SLOT(findBtnClicked()));
	connect(reset_btn, SIGNAL(clicked()), this, SLOT(resetBtnClicked()));
	connect(cancel_btn, SIGNAL(clicked()), this, SLOT(cancelBtnClicked()));

	QGridLayout *grid = new QGridLayout(this, 6, 4, 5, 5);
	grid->addMultiCellWidget(from_hb, 0, 0, 0, 3);
	grid->addMultiCellWidget(to_hb, 1, 1, 0, 3);
	grid->addMultiCellWidget(criteria_bg, 2, 3, 0, 1);
	grid->addMultiCellWidget(phrase_hgb, 2, 2, 2, 3);
	grid->addMultiCellWidget(status_hgb, 3, 3, 2, 3);	
	grid->addMultiCellWidget(reverse_chb, 4, 4, 0, 3, Qt::AlignLeft);
	grid->addWidget(find_btn, 5, 1);
	grid->addWidget(reset_btn, 5, 2);
	grid->addWidget(cancel_btn, 5, 3);

	phrase_edit->setFocus();
}

int daysForMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void HistorySearch::correctFromDays(int index) {
	if (daysForMonth[index] != from_day_cob->count()) {
		QStringList dayslist;
		for (int i = 1; i <= daysForMonth[index]; i++)
			dayslist.append(numslist[i]);
		int current_day = from_day_cob->currentItem();
		from_day_cob->clear();
		from_day_cob->insertStringList(dayslist);
		if (current_day <= from_day_cob->count())
			from_day_cob->setCurrentItem(current_day);
		}
}

void HistorySearch::correctToDays(int index) {
	if (daysForMonth[index] != to_day_cob->count()) {
		QStringList dayslist;
		for (int i = 1; i <= daysForMonth[index]; i++)
			dayslist.append(numslist[i]);
		int current_day = to_day_cob->currentItem();
		to_day_cob->clear();
		to_day_cob->insertStringList(dayslist);
		if (current_day <= to_day_cob->count())
			to_day_cob->setCurrentItem(current_day);
		}
}

void HistorySearch::fromToggled(bool on) {
	from_hgb->setEnabled(on);
}

void HistorySearch::toToggled(bool on) {
	to_hgb->setEnabled(on);
}

void HistorySearch::criteriaChanged(int id) {
	phrase_hgb->setEnabled(id == 1);
	status_hgb->setEnabled(id != 1);
}

void HistorySearch::findBtnClicked() {
	accept();
}

void HistorySearch::cancelBtnClicked() {
	reject();
}

void HistorySearch::resetFromDate() {
	QValueList<HistoryEntry> entries;
	
	entries = history.getHistoryEntries(uins, 0, 1);
	if (entries.count()) {
		from_day_cob->setCurrentItem(entries[0].date.date().day() - 1);
		from_month_cob->setCurrentItem(entries[0].date.date().month() - 1);
		from_year_cob->setCurrentItem(entries[0].date.date().year() - 2000);
		from_hour_cob->setCurrentItem(entries[0].date.time().hour());
		from_min_cob->setCurrentItem(entries[0].date.time().minute());
		correctFromDays(entries[0].date.date().month() - 1);
		}
}

void HistorySearch::resetToDate() {
	QValueList<HistoryEntry> entries;
	
	entries = history.getHistoryEntries(uins, history.getHistoryEntriesCount(uins) - 1, 1);
	if (entries.count()) {
		to_day_cob->setCurrentItem(entries[0].date.date().day() - 1);
		to_month_cob->setCurrentItem(entries[0].date.date().month() - 1);
		to_year_cob->setCurrentItem(entries[0].date.date().year() - 2000);
		to_hour_cob->setCurrentItem(entries[0].date.time().hour());
		to_min_cob->setCurrentItem(entries[0].date.time().minute());
		correctToDays(entries[0].date.date().month() - 1);
		}
}

void HistorySearch::resetBtnClicked() {
	from_hgb->setEnabled(false);
	from_chb->setChecked(false);
	resetFromDate();
	to_chb->setChecked(false);
	to_hgb->setEnabled(false);
	resetToDate();
	criteria_bg->setButton(1);
	phrase_edit->text().truncate(0);
	status_cob->setCurrentItem(0);
	criteriaChanged(1);
	reverse_chb->setChecked(false);
}

void HistorySearch::setDialogValues(HistoryFindRec &findrec) {
	int status;

	from_chb->setChecked(!findrec.fromdate.isNull());
	from_hgb->setEnabled(!findrec.fromdate.isNull());
	if (findrec.fromdate.isNull())
		resetFromDate();
	else {
		from_day_cob->setCurrentItem(findrec.fromdate.date().day() - 1);
		from_month_cob->setCurrentItem(findrec.fromdate.date().month() - 1);
		from_year_cob->setCurrentItem(findrec.fromdate.date().year() - 2000);
		from_hour_cob->setCurrentItem(findrec.fromdate.time().hour());
		from_min_cob->setCurrentItem(findrec.fromdate.time().minute());
		correctFromDays(findrec.fromdate.date().month() - 1);
		}
	to_chb->setChecked(!findrec.todate.isNull());
	to_hgb->setEnabled(!findrec.todate.isNull());
	if (findrec.todate.isNull())
		resetToDate();
	else {
		to_day_cob->setCurrentItem(findrec.todate.date().day() - 1);
		to_month_cob->setCurrentItem(findrec.todate.date().month() - 1);
		to_year_cob->setCurrentItem(findrec.todate.date().year() - 2000);
		to_hour_cob->setCurrentItem(findrec.todate.time().hour());
		to_min_cob->setCurrentItem(findrec.todate.time().minute());
		correctToDays(findrec.todate.date().month() - 1);
		}
	criteria_bg->setButton(findrec.type);
	criteriaChanged(findrec.type);
	switch (findrec.type) {
		case 1:
			phrase_edit->setText(findrec.data);
			break;
		case 2:
			if (findrec.data == "avail")
				status = 0;
			else
				if (findrec.data == "busy")
					status = 1;
				else
					if (findrec.data == "invisible")
						status = 2;
					else
						if (findrec.data == "notavail")
							status = 3;
			status_cob->setCurrentItem(status);
			break;
		}
	reverse_chb->setChecked(findrec.reverse);
}

HistoryFindRec HistorySearch::getDialogValues() {
	HistoryFindRec findrec;

	if (from_chb->isChecked()) {
		findrec.fromdate.setDate(QDate(from_year_cob->currentItem() + 2000,
			from_month_cob->currentItem() + 1, from_day_cob->currentItem() + 1));
		findrec.fromdate.setTime(QTime(from_hour_cob->currentItem(), from_min_cob->currentItem()));
		}
	if (to_chb->isChecked()) {
		findrec.todate.setDate(QDate(to_year_cob->currentItem() + 2000,
			to_month_cob->currentItem() + 1, to_day_cob->currentItem() + 1));
		findrec.todate.setTime(QTime(to_hour_cob->currentItem(), to_min_cob->currentItem()));
		}
	findrec.type = criteria_bg->id(criteria_bg->selected());
	switch (findrec.type) {
		case 1:
			findrec.data = phrase_edit->text();
			break;
		case 2:
			switch (status_cob->currentItem()) {
				case 0:
					findrec.data = "avail";
					break;
				case 1:
					findrec.data = "busy";
					break;
				case 2:
					findrec.data = "invisible";
					break;
				case 3:
					findrec.data = "notavail";
					break;
				}
			break;
		}
	findrec.reverse = reverse_chb->isChecked();
	return findrec;
}

HistoryManager history;
