/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include <qdialog.h>
#include <qtextbrowser.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include "libgadu.h"
#include "misc.h"

#define	HISTORYMANAGER_ENTRY_CHATSEND	0x00000001
#define	HISTORYMANAGER_ENTRY_CHATRCV	0x00000002
#define	HISTORYMANAGER_ENTRY_MSGSEND	0x00000004
#define	HISTORYMANAGER_ENTRY_MSGRCV	0x00000008
#define	HISTORYMANAGER_ENTRY_STATUS	0x00000010
#define	HISTORYMANAGER_ENTRY_SMSSEND	0x00000020
#define HISTORYMANAGER_ENTRY_ALL	0x0000003f

struct HistoryEntry {
	int type;
	uin_t uin;
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString message;
	unsigned int status;
	QString ip;
	QString description;
	QString mobile;	
};

class History : public QDialog {
	Q_OBJECT
	public:
		History(UinsList uins);

	public slots:
		void prevBtnClicked();
		void nextBtnClicked();

	protected:
		void formatHistoryEntry(QString &text, const HistoryEntry &entry);
		void showHistoryEntries(int from, int count);

		QTextBrowser *body;
		UinsList uins;
		int start;
};

class HistoryManager {
	public:
		void appendMessage(UinsList receivers, uin_t sender, const QString &msg, bool own, time_t=0, bool chat=true);
		void appendSms(const QString &mobile, const QString &msg);
		void appendStatus(uin_t uin, unsigned int status, QString description = QString::null);

		void convHist2ekgForm(UinsList uins);
		void convSms2ekgForm();
		void buildIndex(UinsList uins);
		void buildIndex(QString mobile = QString::null);
		int getHistoryEntriesCount(UinsList uins);
		int getHistoryEntriesCount(QString mobile = QString::null);
		QValueList<HistoryEntry> getHistoryEntries(UinsList uins, int from, int count, int mask = HISTORYMANAGER_ENTRY_ALL);

		static QString getFileNameByUinsList(UinsList &uins);
		static QStringList mySplit(const QChar &sep, const QString &str);

	private:
		QString text2csv(const QString &text);
		int typeOfLine(const QString &line);
		int getHistoryEntriesCountPrivate(const QString &filename);
		void buildIndexPrivate(const QString &filename);
};

extern HistoryManager history;

#endif
