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

QString HistoryManager::text2csv(const QString &text)
{
	QString csv = text;
	csv.replace(QRegExp("\\\\"), "\\\\");
	csv.replace(QRegExp("\""), "\\\"");
	csv.replace(QRegExp("\n"), "\\n");
	if (csv != text || text.find(QRegExp(","), 0) != -1)
		csv = QString("\"") + csv + QString("\"");	
	return csv;
}

void HistoryManager::appendMessage(UinsList uins, uin_t uin, const QString &msg, bool own, time_t time, bool chat) {
	QFile f;
	QString fname = ggPath("history/");
	QString line, nick;
	QStringList linelist;
	int i;

	convHist2ekgForm(uins);

	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));
		if (i < uins.count() - 1)
			fname.append("_");
		}
		
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
	linelist.append(QString::number(-QDateTime::currentDateTime().secsTo(
		QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
	if (!own)
		linelist.append(QString::number(time));
	linelist.append(text2csv(msg));
	line = linelist.join(",");

	f.setName(fname);
	if (!(f.open(IO_WriteOnly | IO_Append))) {
		kdebug("HistoryManager::appendMessage(): Error opening history file %s\n", (const char *)fname.local8Bit());
		return;
		}

	QTextStream stream(&f);
	stream.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	stream << line << '\n';

	f.close();
}

void HistoryManager::appendSms(const QString &mobile, const QString &msg)
{
	QFile f;
	QTextStream stream;
	QStringList linelist;
	QString altnick, line, fname;
	uin_t uin = 0;

	kdebug("HistoryManager::appendSms(): appending sms to history (%s)\n", mobile.local8Bit().data());
	
	convSms2ekgForm();

	linelist.append("smssend");
	linelist.append(mobile);
	linelist.append(QString::number(-QDateTime::currentDateTime().secsTo(
		QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
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
	QFile f;
	QString fname = ggPath("history/");
	QString line, nick, addr;
	QStringList linelist;
	int ip;
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
	linelist.append(QString::number(-QDateTime::currentDateTime().secsTo(
		QDateTime(QDate(1970, 1, 1), QTime(0 ,0)))));
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
	QStringList linelist;
	uin_t uin;
	int i;

	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));
		if (i < uins.count() - 1)
			fname.append("_");
		}

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
	QRegExp sep("\\s");
	our = foreign = false;
	while ((line = stream.readLine()) != QString::null) {
		our = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
		foreign = !line.find(QRegExp("^\\S+\\s::\\s\\d{2,2}\\s\\d{2,2}\\s\\d{4,4},\\s\\(\\d{2,2}:\\d{2,2}:\\d{2,2}\\s/\\sS\\s\\d{2,2}:\\d{2,2}:\\d{2,2}\\)$"));
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
		f.close();
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		fout.remove();
		f.close();
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
		kdebug("HistoryManager::convHist2ekgForm(): Error opening sms history file %s\n", (const char *)fname.local8Bit());
		return;
		}
	fnameout = fname + ".new";
	fout.setName(path + fnameout);
	if (!(fout.open(IO_WriteOnly | IO_Truncate))) {
		kdebug("HistoryManager::convHist2ekgForm(): Error opening new sms history file %s\n", (const char *)fnameout.local8Bit());
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
		f.close();
		fout.close();
		QDir dir(path);
		dir.rename(fname, fname + QString(".old"));
		dir.rename(fnameout, fname);
		}
	else {
		fout.remove();
		f.close();
		}
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
	uins.sort();
	for (i = 0; i < uins.count(); i++) {
		fname.append(QString::number(uins[i]));	
		if (i < uins.count() - 1)
			fname.append("_");
		}
		
	QFile f(fname);
	if (f.open(IO_ReadOnly)) {
		QTextStream t(&f);
		t.setCodec(QTextCodec::codecForName("ISO 8859-2"));
		body->setText(t.read());
		}
	else {
		kdebug("History(): Error opening history file %s\n", (const char *)fname.local8Bit());
		body->setText(i18n("Error opening history file"));
		}
	grid->addWidget(body,0,0);
	grid->addWidget(closebtn,1,0, Qt::AlignRight);

	connect(closebtn, SIGNAL(clicked()), this, SLOT(close()));

	resize(500,400);
}

HistoryManager history;
