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
#include <qmultilineedit.h>
#include <qstring.h>

#include "libgadu.h"
#include "misc.h"

class History : public QDialog {
	Q_OBJECT
	public:
		History(UinsList uins);

	protected:
		QMultiLineEdit *body;
};

class HistoryManager {
	public:
		void appendMessage(UinsList receivers, uin_t sender, const QString &msg, bool own, time_t=0, bool chat=true);
		void appendSms(const QString &mobile, const QString &msg);
		void appendStatus(uin_t uin, unsigned int status, QString description=QString::null);

		void convHist2ekgForm(UinsList uins);
		void convSms2ekgForm();
	private:
		QString text2csv(const QString &text);
		int typeOfLine(const QString &line);
};

extern HistoryManager history;

#endif
