/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qsplitter.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qvbuttongroup.h>

// #include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "gadu_images_manager.h"
#include "history.h"
#include "kadu.h"
#include "kadu_text_browser.h"
#include "html_document.h"
#include "misc.h"

enum {
	HISTORYMANAGER_ORDINARY_LINE,
	HISTORYMANAGER_HISTORY_OUR,
	HISTORYMANAGER_HISTORY_FOREIGN,
	HISTORYMANAGER_SMS_WITH_NICK,
	HISTORYMANAGER_SMS_WITHOUT_NICK
};

HistoryManager::HistoryManager(QObject *parent, const char *name) : QObject(parent, name), bufferedMessages(), imagesTimer(new QTimer(this, "imagesTimer"))
{
	imagesTimer->start(1000*60);//60 sekund
	connect(imagesTimer, SIGNAL(timeout()), this, SLOT(checkImagesTimeouts()));
	connect(userlist, SIGNAL(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)),
		this, SLOT(statusChanged(UserListElement, QString, const UserStatus &, bool, bool)));
}

QString HistoryManager::text2csv(const QString &text)
{
	QString csv = text;
	csv.replace("\\", "\\\\");
	csv.replace("\"", "\\\"");
	csv.replace("\r\n", "\\n");
	csv.replace("\n", "\\n");
	if (csv != text || text.find(',', 0) != -1)
		csv = QString("\"%1\"").arg(csv);
	return csv;
}

QString HistoryManager::getFileNameByUinsList(UinsList uins)
{
	kdebugf();
	QString fname;
	if (!uins.isEmpty())
	{
		uins.sort();
		unsigned int i = 0, uinsCount = uins.count();
		CONST_FOREACH(uin, uins)
		{
			fname.append(QString::number(*uin));
			if (i++ < uinsCount - 1)
				fname.append("_");
		}
	}
	else
		fname = "sms";
	kdebugf2();
	return fname;
}

void HistoryManager::appendMessage(UinsList uins, UinType uin, const QString &msg, bool own, time_t czas, bool chat, time_t arriveTime)
{
	kdebugf();
	QFile f, fidx;
	QString fname = ggPath("history/");
	QString line, nick;
	QStringList linelist;
	int offs;

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
	if (userlist->contains("Gadu", QString::number(uin)))
		nick = userlist->byID("Gadu", QString::number(uin)).altNick();
	else
		nick = QString::number(uin);
	linelist.append(text2csv(nick));
	linelist.append(QString::number(arriveTime));
	if (!own)
		linelist.append(QString::number(czas));
	linelist.append(text2csv(msg));
	line = linelist.join(",");

	f.setName(fname);
	if (!(f.open(IO_WriteOnly | IO_Append)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
	}

	buildIndexPrivate(fname);
	fidx.setName(f.name() + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append))
	{
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
	}

	QTextStream stream(&f);
	stream.setCodec(codec_latin2);
	stream << line << '\n';

	f.close();
	kdebugf2();
}

void HistoryManager::appendSms(const QString &mobile, const QString &msg)
{
	kdebugmf(KDEBUG_FUNCTION_START, "appending sms to history (%s)\n", mobile.local8Bit().data());
	QFile f, fidx;
	QTextStream stream;
	QStringList linelist;
	QString altnick, line, fname;
	UinType uin = 0;
	int offs;

	QString html_msg = msg;
	HtmlDocument::escapeText(html_msg);

	convSms2ekgForm();

	linelist.append("smssend");
	linelist.append(mobile);
	linelist.append(QString::number(time(NULL)));
	linelist.append(text2csv(html_msg));

	CONST_FOREACH(i, *userlist)
		if ((*i).mobile() == mobile)
		{
			altnick = (*i).altNick();
			uin = (*i).ID("Gadu").toUInt();;
			break;
		}
	if (uin)
	{
		UinsList uins(uin);
		convHist2ekgForm(uins);
		linelist.append(text2csv(altnick));
		linelist.append(QString::number(uin));
	}

	line = linelist.join(",");

	f.setName(ggPath("history/sms"));
	if (!(f.open(IO_WriteOnly | IO_Append)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening sms history file\n");
		return;
	}

	buildIndexPrivate(f.name());
	fidx.setName(f.name() + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append))
	{
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
	}

	stream.setDevice(&f);
	stream.setCodec(codec_latin2);
	stream << line << '\n';
	f.close();

	if (uin)
	{
		fname = ggPath("history/");
		fname = fname + QString::number(uin);
		f.setName(fname);
		if (!(f.open(IO_WriteOnly | IO_Append)))
		{
			kdebugmf(KDEBUG_ERROR, "Error opening sms history\n");
			return;
		}

		fidx.setName(f.name() + ".idx");
		if (fidx.open(IO_WriteOnly | IO_Append))
		{
			offs = f.at();
			fidx.writeBlock((const char *)&offs, sizeof(int));
			fidx.close();
		}

		stream.setDevice(&f);
		stream.setCodec(codec_latin2);
		stream << line << '\n';
		f.close();
	}
	kdebugf2();
}

void HistoryManager::appendStatus(UinType uin, const UserStatus &status)
{
	kdebugf();

	QFile f, fidx;
	QString fname = ggPath("history/");
	QString line, nick, addr;
	QStringList linelist;
	int offs;
	QHostAddress ip;
	unsigned short port;
//	struct in_addr in;

	if (config_file.readBoolEntry("History", "DontSaveStatusChanges"))
	{
		kdebugm(KDEBUG_INFO|KDEBUG_FUNCTION_END, "not appending\n");
		return;
	}

	UinsList uins(uin);
	convHist2ekgForm(uins);
	linelist.append("status");
	linelist.append(QString::number(uin));
	if (userlist->contains("Gadu", QString::number(uin), FalseForAnonymous))
	{
		UserListElement user = userlist->byID("Gadu", QString::number(uin));
		nick = user.altNick();
		ip = user.IP("Gadu");
		port = user.port("Gadu");
	}
	else
	{
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
	switch (status.status())
	{
		case Online:
			linelist.append("avail");
			break;
		case Busy:
			linelist.append("busy");
			break;
		case Invisible:
			linelist.append("invisible");
			break;
		case Offline:
		default:
			linelist.append("notavail");
			break;
	}
	if (status.hasDescription())
	{
		QString d = status.description();
		HtmlDocument::escapeText(d);
		linelist.append(text2csv(d));
	}
	line = linelist.join(",");

	fname = fname + QString::number(uin);
	f.setName(fname);
	if (!(f.open(IO_WriteOnly | IO_Append)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
	}

	buildIndexPrivate(fname);
	fidx.setName(fname + ".idx");
	if (fidx.open(IO_WriteOnly | IO_Append))
	{
		offs = f.at();
		fidx.writeBlock((const char *)&offs, sizeof(int));
		fidx.close();
	}

	QTextStream stream(&f);
	stream.setCodec(codec_latin2);
	stream << line << '\n';

	f.close();
	kdebugf2();
}

void HistoryManager::removeHistory(const UinsList &uins)
{
	kdebugf();

	QString fname;
	switch (QMessageBox::information(kadu, "Kadu", qApp->translate("@default",QT_TR_NOOP("Clear history?")),
		qApp->translate("@default",QT_TR_NOOP("Yes")), qApp->translate("@default",QT_TR_NOOP("No")), QString::null, 1, 1))
	{
		case 0:
			fname = ggPath("history/");
			fname.append(getFileNameByUinsList(uins));
			kdebugmf(KDEBUG_INFO, "deleting %s\n", (const char *)fname.local8Bit());
			QFile::remove(fname);
			QFile::remove(fname + ".idx");
			break;
		case 1:
			break;
	}
	kdebugf2();
}

void HistoryManager::convHist2ekgForm(UinsList uins)
{
	kdebugf();

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QStringList linelist;
	UinType uin;

	uins.sort();//nie wiem czy to jest konieczne...
	fname = getFileNameByUinsList(uins);

	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
	}

	fnameout = fname + ".new";
	fout.setName(path + fnameout);
	if (!(fout.open(IO_WriteOnly | IO_Truncate)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening new history file %s\n", (const char *)fnameout.local8Bit());
		f.close();
		return;
	}

	QTextStream stream(&f);
	stream.setCodec(codec_latin2);
	QTextStream streamout(&fout);
	streamout.setCodec(codec_latin2);

	bool our, foreign;
	QString dzien, miesiac, rok, czas, sczas, text, temp, lineout;
	QDateTime datetime, sdatetime;
	QRegExp sep("\\s"), sep2("::");
	our = foreign = false;
	UinType myUin=config_file.readNumEntry("General","UIN");
	while ((line = stream.readLine()) != QString::null)
	{
//		our = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		our = !line.find(QRegExp("^(\\S|\\s)+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
//		foreign = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		foreign = !line.find(QRegExp("^(\\S|\\s)+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		if (our || foreign)
		{
			if (!linelist.isEmpty())
			{
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
			if (our)
			{
				czas.truncate(czas.length() - 1);
				linelist.append("chatsend");
			}
			datetime.setTime(QTime(czas.left(2).toInt(), czas.mid(3, 2).toInt(), czas.right(2).toInt()));
			if (foreign)
			{
				sczas = line.section(sep, 6, 6);
				sczas.truncate(sczas.length() - 1);
				sdatetime.setTime(QTime(sczas.left(2).toInt(), sczas.mid(3, 2).toInt(), sczas.right(2).toInt()));
				linelist.append("chatrcv");
			}
			if (our)
			{
				if (uins.count() > 1)
					uin = 0;
				else if (myUin != uins[0])
					uin = uins[0];
				else
					uin = uins[1];
			}
			else if (userlist->containsAltNick(nick))
				uin = userlist->byAltNick(nick).ID("Gadu").toUInt();
			else if (uins.count() > 1)
				uin = 0;
			else if (myUin != uins[0])
				uin = uins[0];
			else
				uin = uins[1];
			linelist.append(QString::number(uin));
			if (our)
				if (userlist->contains("Gadu", QString::number(uin)))
					nick = userlist->byID("Gadu", QString::number(uin)).altNick();
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
		else
		{
			if (linelist.isEmpty())
				break;
			text.append(line);
			text.append("\n");
		}
	}
	if (!linelist.isEmpty())
	{
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
	else
	{
		f.close();
		fout.remove();
	}
	kdebugf2();
}

void HistoryManager::convSms2ekgForm()
{
	kdebugf();

	QFile f, fout;
	QString path = ggPath("history/");
	QString fname, fnameout, line, nick;
	QStringList linelist;
	UinType uin=0;

	fname = "sms";
	f.setName(path + fname);
	if (!(f.open(IO_ReadWrite)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening sms history file %s\n", (const char *)fname.local8Bit());
		return;
	}
	fnameout = fname + ".new";
	fout.setName(path + fnameout);
	if (!(fout.open(IO_WriteOnly | IO_Truncate)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening new sms history file %s\n", (const char *)fnameout.local8Bit());
		f.close();
		return;
	}

	QTextStream stream(&f);
	stream.setCodec(codec_latin2);
	QTextStream streamout(&fout);
	streamout.setCodec(codec_latin2);

	bool header;
	QString mobile, dzien, miesiac, rok, czas, text, temp, lineout;
	QDateTime datetime;
	QRegExp sep("\\s");
	header = false;
	while ((line = stream.readLine()) != QString::null)
	{
		header = !line.find(QRegExp("^\\S+\\s\\(\\d+\\)\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		if (header)
		{
			if (!linelist.isEmpty())
			{
				text.truncate(text.length() - 1);
				if (text[text.length() - 1] == '\n')
					text.truncate(text.length() - 1);
				linelist.append(text2csv(text));
				if (uin)
				{
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
			CONST_FOREACH(user, *userlist)
				if ((*user).mobile() == mobile)
					uin = (*user).ID("Gadu").toUInt();
			header = false;
		}
		else
		{
			if (linelist.isEmpty())
				break;
			text.append(line);
			text.append("\n");
		}
	}
	if (!linelist.isEmpty())
	{
		text.truncate(text.length() - 1);
		if (text[text.length() - 1] == '\n')
			text.truncate(text.length() - 1);
		linelist.append(text2csv(text));
		if (uin)
		{
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
	else
	{
		f.close();
		fout.remove();
	}
	kdebugf2();
}

int HistoryManager::getHistoryEntriesCountPrivate(const QString &filename) const
{
	kdebugf();

	int lines;
	QFile f;
	QString path = ggPath("history/");
	QByteArray buffer;

	f.setName(path + filename + ".idx");
	if (!(f.open(IO_ReadOnly)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)filename.local8Bit());
		return 0;
	}
	lines = f.size() / sizeof(int);
//	buffer = f.readAll();
	f.close();
//	lines = buffer.contains('\n');

	kdebugmf(KDEBUG_INFO, "%d lines\n", lines);
	return lines;
}

int HistoryManager::getHistoryEntriesCount(const UinsList &uins)
{
	kdebugf();
	convHist2ekgForm(uins);
	buildIndex(uins);
	int ret=getHistoryEntriesCountPrivate(getFileNameByUinsList(uins));
	kdebugf2();
	return ret;
}

int HistoryManager::getHistoryEntriesCount(const QString &mobile)
{
	kdebugf();
	convSms2ekgForm();
	buildIndex();
	int ret;
	if (mobile == QString::null)
		ret= getHistoryEntriesCountPrivate("sms");
	else
		ret= getHistoryEntriesCountPrivate(mobile);
	kdebugf2();
	return ret;
}

QValueList<HistoryEntry> HistoryManager::getHistoryEntries(UinsList uins, int from, int count, int mask)
{
	kdebugf();

	QValueList<HistoryEntry> entries;
	QStringList tokens;
	QFile f, fidx;
	QString path = ggPath("history/");
	QString filename, line;
	int offs;

	if (!uins.isEmpty())
		filename = getFileNameByUinsList(uins);
	else
		filename = "sms";
	f.setName(path + filename);
	if (!(f.open(IO_ReadOnly)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)filename.local8Bit());
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
	stream.setCodec(codec_latin2);

	int linenr = from;

	struct HistoryEntry entry;
//	int num = 0;
	while (linenr < from + count && (line = stream.readLine()) != QString::null)
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
						entry.status = GG_STATUS_AVAIL;
					else if (tokens[5] == "notavail")
						entry.status = GG_STATUS_NOT_AVAIL;
					else if (tokens[5] == "busy")
						entry.status = GG_STATUS_BUSY;
					else if (tokens[5] == "invisible")
						entry.status = GG_STATUS_INVISIBLE;
					if (tokens.count() == 7)
					{
						switch (entry.status)
						{
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

uint HistoryManager::getHistoryDate(QTextStream &stream)
{
	kdebugf();
	QString line;
	static QStringList types = QStringList::split(" ", "smssend chatrcv chatsend msgrcv msgsend status");
	QStringList tokens;
	int type, pos;

	line = stream.readLine();
	tokens = mySplit(',', line);
	type = types.findIndex(tokens[0]);
	if (!type)
		pos = 2;
	else
		if (type < 5)
			pos = 3;
		else
			pos = 4;
	kdebugf2();
	return (tokens[pos].toUInt() / 86400);
}

QValueList<HistoryDate> HistoryManager::getHistoryDates(const UinsList &uins)
{
	kdebugf();

	QValueList<HistoryDate> entries;
	HistoryDate newdate;
	QFile f, fidx;
	QString path = ggPath("history/");
	QString filename, line;
	uint offs, count, oldidx, actidx, leftidx, rightidx, /*mididx,*/ olddate, actdate, jmp;
//	uint num = 0;

	if (!uins.isEmpty())
		count = getHistoryEntriesCount(uins);
	else
		count = getHistoryEntriesCount("sms");
	if (!count)
		return entries;

	filename = getFileNameByUinsList(uins);
	f.setName(path + filename);
	if (!(f.open(IO_ReadOnly)))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file %s\n", (const char *)filename.local8Bit());
		return entries;
	}
	QTextStream stream(&f);
	stream.setCodec(codec_latin2);

	fidx.setName(f.name() + ".idx");
	if (!fidx.open(IO_ReadOnly))
		return entries;

	oldidx = actidx = 0;
	olddate = actdate = getHistoryDate(stream);
	kdebugmf(KDEBUG_INFO, "actdate = %d\n", actdate);
	newdate.idx = 0;
	newdate.date.setTime_t(actdate * 86400);
	entries.append(newdate);

	while (actidx < count - 1)
	{
		jmp = 1;
		do
		{
			oldidx = actidx;
			actidx += jmp;
			jmp <<= 1;
			if (jmp > 128)
				jmp = 128;
			if (actidx >= count)
				actidx = count - 1;
			if (actidx == oldidx)
				break;
			fidx.at(actidx * sizeof(int));
			fidx.readBlock((char *)&offs, (Q_LONG)sizeof(int));
			f.at(offs);
			actdate = getHistoryDate(stream);
//			if (++num%10 == 0)
//				qApp->processEvents();
		} while (actdate == olddate);

		if (actidx == oldidx)
			break;
		if (actdate > olddate)
		{
			leftidx = oldidx;
			rightidx = actidx;
			while (rightidx - leftidx > 1)
			{
				actidx = (leftidx + rightidx) / 2;
				fidx.at(actidx * sizeof(int));
				fidx.readBlock((char *)&offs, (Q_LONG)sizeof(int));
				f.at(offs);
				actdate = getHistoryDate(stream);
				if (actdate > olddate)
					rightidx = actidx;
				else
					leftidx = actidx;
//				if (++num%10 == 0)
//					qApp->processEvents();
			}
			newdate.idx = actidx = rightidx;
			if (actdate == olddate)
			{
				fidx.at(actidx * sizeof(int));
				fidx.readBlock((char *)&offs, (Q_LONG)sizeof(int));
				f.at(offs);
				actdate = getHistoryDate(stream);
			}
			newdate.date.setTime_t(actdate * 86400);
			entries.append(newdate);
			olddate = actdate;
		}
	}

	fidx.close();
	f.close();

	kdebugf2();
	return entries;
}

QValueList<UinsList> HistoryManager::getUinsLists() const
{
	kdebugf();
	QValueList<UinsList> entries;
	QDir dir(ggPath("history/"), "*.idx");
	QStringList struins;
	UinsList uins;

	QStringList entryList = dir.entryList();
	FOREACH(entry, entryList)
	{
		struins = QStringList::split("_", (*entry).remove(QRegExp(".idx$")));
		uins.clear();
		if (struins[0] != "sms")
			CONST_FOREACH(struin, struins)
				uins.append((*struin).toUInt());
		entries.append(uins);
	}

	kdebugf2();
	return entries;
}

void HistoryManager::buildIndexPrivate(const QString &filename)
{
	kdebugf();
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
	if (!fin.open(IO_ReadOnly))
	{
		kdebugmf(KDEBUG_ERROR, "Error opening history file: %s\n", (const char *)fin.name().local8Bit());
		return;
	}
	if (!fout.open(IO_WriteOnly | IO_Truncate))
	{
		kdebugmf(KDEBUG_ERROR, "Error creating history index file: %s\n", (const char *)fout.name().local8Bit());
		fin.close();
		return;
	}
	inbuf = new char[65536];
	outbuf = new int[4096];

	inoffs = outbufoffs = 0;
	while ((read = fin.readBlock(inbuf, 65536)) > 0)
	{
		inbufoffs = 0;
		while (inbufoffs < read)
		{
			if (saved)
				saved = false;
			else
				outbuf[outbufoffs++] = inoffs + inbufoffs;
			if (outbufoffs == 4096)
			{
				written = fout.writeBlock((char *)outbuf, 4096 * sizeof(int));
				outbufoffs = 0;
			}
			while (inbufoffs < read && inbuf[inbufoffs] != '\n')
				++inbufoffs;
			if (inbufoffs < read)
				++inbufoffs;
			if (inbufoffs == read)
			{
				inoffs += read;
				saved = true;
			}
		}
	}
	if (outbufoffs)
		written = fout.writeBlock((char *)outbuf, outbufoffs * sizeof(int));

	delete []inbuf;
	delete []outbuf;

	fin.close();
	fout.close();
	kdebugf2();
}

void HistoryManager::buildIndex(const UinsList &uins)
{
	kdebugf();
	buildIndexPrivate(ggPath("history/") + getFileNameByUinsList(uins));
	kdebugf2();
}

void HistoryManager::buildIndex(const QString &mobile)
{
	kdebugf();
	if (mobile == QString::null)
		buildIndexPrivate(ggPath("history/") + "sms");
	else
		buildIndexPrivate(ggPath("history/") + mobile);
	kdebugf2();
}

QStringList HistoryManager::mySplit(const QChar &sep, const QString &str)
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
				switch (str[idx + 1])
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
				pos1 = str.find('\\', idx);
				if (pos1 == -1)
					pos1 = strlength;
				pos2 = str.find('"', idx);
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
				pos1 = str.find(sep, idx);
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

int HistoryManager::getHistoryEntryIndexByDate(const UinsList &uins, const QDateTime &date, bool enddate)
{
	kdebugf();

	QValueList<HistoryEntry> entries;
	int count = getHistoryEntriesCount(uins);
	int start, end;

	start = 0;
	end = count - 1;
	while (end - start >= 0)
	{
		kdebugmf(KDEBUG_INFO, "start = %d, end = %d\n", start, end);
		entries = getHistoryEntries(uins, start + ((end - start) / 2), 1);
		if (!entries.isEmpty())
			if (date < entries[0].date)
				end -= ((end - start) / 2) + 1;
			else if (date > entries[0].date)
				start += ((end - start) / 2) + 1;
			else
				return start + ((end - start) / 2);
	}
	if (end < 0)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "return 0\n");
		return 0;
	}
	if (start >= count)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "return count=%d\n", count);
		return count;
	}
	if (enddate)
	{
		entries = getHistoryEntries(uins, start, 1);
		if (!entries.isEmpty() && date < entries[0].date)
			--start;
	}
	kdebugmf(KDEBUG_FUNCTION_END, "return %d\n", start);
	return start;
}

void HistoryManager::chatMsgReceived(Protocol * /*protocol*/, UserListElements senders, const QString& msg, time_t t, bool& /*grab*/)
{
	if (!config_file.readBoolEntry("History", "Logging"))
		return;
	kdebugf();
	int occur = msg.contains(QRegExp("<img [^>]* gg_crc[^>]*>"));
	UinType sender0 = senders[0].ID("Gadu").toUInt();
	kdebugm(KDEBUG_INFO, "sender: %d msg: '%s' occur:%d\n", sender0, msg.local8Bit().data(), occur);
	UinsList uins;//TODO: throw out UinsList as soon as possible!
	CONST_FOREACH(u, senders)
		if ((*u).usesProtocol("Gadu"))
			uins.append((*u).ID("Gadu").toUInt());
	if (bufferedMessages.find(sender0) != bufferedMessages.end() || occur > 0)
	{
		kdebugm(KDEBUG_INFO, "buffering\n");
		bufferedMessages[sender0].append(BuffMessage(uins, msg, t, time(NULL), false, occur));
		checkImageTimeout(sender0);
	}
	else
	{
		kdebugm(KDEBUG_INFO, "appending to history\n");
		appendMessage(uins, sender0, msg, false, t, true, time(NULL));
	}
	kdebugf2();
}

void HistoryManager::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path)
{
	if (!config_file.readBoolEntry("History", "Logging"))
		return;
	kdebugf();
	kdebugm(KDEBUG_INFO, "sender: %d, size: %d, crc:%u, path:%s\n", sender, size, crc32, path.local8Bit().data());
	QString reg = GaduImagesManager::loadingImageHtml(sender, size, crc32);
	QString imagehtml = GaduImagesManager::imageHtml(path);
	QMap<UinType, QValueList<BuffMessage> >::iterator it = bufferedMessages.find(sender);
	if (it != bufferedMessages.end())
	{
//		kdebugm(KDEBUG_INFO, "sender found\n");
		QValueList<BuffMessage> &messages = it.data();
		FOREACH(msg, messages)
		{
//			kdebugm(KDEBUG_INFO, "counter:%d\n", (*msg).counter);
			if ((*msg).counter)
			{
				int occur = (*msg).message.contains(reg);
//				kdebugm(KDEBUG_INFO, "occur:%d\n", occur);
				if (occur)
				{
					(*msg).message.replace(reg, imagehtml);
					(*msg).counter -= occur;
				}
			}
		}
//		kdebugm(KDEBUG_INFO, "> msgs.size():%d\n", messages.size());
		while (!messages.isEmpty())
		{
			BuffMessage &msg = messages.front();
			if (msg.counter > 0)
				break;
			appendMessage(msg.uins, msg.uins[0], msg.message, msg.own, msg.tm, true, msg.arriveTime);
			messages.pop_front();
		}
//		kdebugm(KDEBUG_INFO, ">> msgs.size():%d\n", messages.size());
		if (messages.isEmpty())
			bufferedMessages.remove(sender);
	}
	kdebugf2();
}

void HistoryManager::addMyMessage(const UinsList &senders, const QString &msg)
{
	if (!config_file.readBoolEntry("History", "Logging"))
		return;
	kdebugf();
	time_t current=time(NULL);
	if (bufferedMessages.find(senders[0])!=bufferedMessages.end())
	{
		bufferedMessages[senders[0]].append(BuffMessage(senders, msg, 0, current, true, 0));
		checkImageTimeout(senders[0]);
	}
	else
		appendMessage(senders, senders[0], msg, true, 0, true, current);
	kdebugf2();
}

void HistoryManager::checkImageTimeout(UinType uin)
{
	kdebugf();
	time_t currentTime = time(NULL);
	QValueList<BuffMessage> &msgs = bufferedMessages[uin];
	while (!msgs.isEmpty())
	{
		BuffMessage &msg = msgs.front();
		kdebugm(KDEBUG_INFO, "arriveTime:%ld current:%ld counter:%d\n", msg.arriveTime, currentTime, msg.counter);
		if (msg.arriveTime + 60 < currentTime || msg.counter == 0)
		{
			kdebugm(KDEBUG_INFO, "moving message to history\n");
			appendMessage(msg.uins, msg.uins[0], msg.message, msg.own, msg.tm, true, msg.arriveTime);
			msgs.pop_front();
		}
		else
		{
			kdebugm(KDEBUG_INFO, "it's too early\n");
			break;
		}
	}
	if (msgs.isEmpty())
		bufferedMessages.remove(uin);
	kdebugf2();
}

void HistoryManager::checkImagesTimeouts()
{
	kdebugf();
	QValueList<UinType> uins = bufferedMessages.keys();

	CONST_FOREACH(uin, uins)
		checkImageTimeout(*uin);
	kdebugf2();
}

void HistoryManager::statusChanged(UserListElement elem, QString protocolName,
					const UserStatus & /*oldStatus*/, bool /*massively*/, bool /*last*/)
{
	if (protocolName == "Gadu") //TODO: make more general
		appendStatus(elem.ID("Gadu").toUInt(), elem.status("Gadu"));
}

HistoryEntry::HistoryEntry() :
	type(0), uin(0), nick(), date(), sdate(),
	message(), status(0), ip(), description(), mobile()
{
}

HistoryDate::HistoryDate() : date(), idx(0)
{
}

HistoryManager *history = 0;
