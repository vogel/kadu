/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "status.h"
#include "kadu.h"
#include <qfile.h>
#include <qtextstream.h>

AutoAwayTimer* AutoAwayTimer::autoaway_object=NULL;

int gg_statuses[] = {GG_STATUS_AVAIL, GG_STATUS_AVAIL_DESCR, GG_STATUS_BUSY, GG_STATUS_BUSY_DESCR,
	GG_STATUS_INVISIBLE, GG_STATUS_INVISIBLE_DESCR, GG_STATUS_NOT_AVAIL, GG_STATUS_NOT_AVAIL_DESCR,
	GG_STATUS_BLOCKED};

const char *statustext[] = {"Online", "Online (d.)",
	"Busy", "Busy (d.)",
	"Invisible", "Invisible (d.)",
	"Offline", "Offline (d.)", "Blocking"};

/* sprawdza czy nasz status jest opisowy
 odporne na podanie status'u z maska dla przyjaciol */
bool ifStatusWithDescription(int status) {
	status = status & (~GG_STATUS_FRIENDS_MASK);

	return (status == GG_STATUS_AVAIL_DESCR || status == GG_STATUS_NOT_AVAIL_DESCR ||
		status == GG_STATUS_BUSY_DESCR || status == GG_STATUS_INVISIBLE_DESCR);
}

/* zwraca nasz aktualny status 
 jesli stan sesji jest inny niz polaczone to znaczy
 ze jestesmy niedostepni */
int getActualStatus() {
	if (sess && sess->state == GG_STATE_CONNECTED)
		return sess->status;

	return GG_STATUS_NOT_AVAIL;
}

int statusGGToStatusNr(int status) {
	int i = 0;
	if (status == GG_STATUS_INVISIBLE2)
		return 4;
	while (i < 9 && gg_statuses[i] != status)
		i++;
	if (i < 9)
		return i;
	return -1;
}

AutoStatusTimer::AutoStatusTimer(QObject* parent)
	: QTimer(parent,"AutoStatusTimer")
{
	length_buffor=0;
	
	connect(this, SIGNAL(timeout()), SLOT(onTimeout()));
//	start(1000, TRUE);
}

void AutoStatusTimer::onTimeout()
{
	if (sess && ifStatusWithDescription(sess->status) && config.addtodescription) {
		QFile f(preparePath("description"));
		if (!f.open(IO_ReadOnly)) {
			start(1000, TRUE);
			return;
			}
		QTextStream s(&f);
		QString new_description;
		new_description = s.readLine();
		f.close();
		f.remove(preparePath("description"));
		//if (new_description != own_description) {
			fprintf(stderr, "AutoStatus: adding \"%s\" to description\n", new_description.local8Bit().data());
			//own_description = new_description;
			own_description.truncate(own_description.length() - length_buffor);
			own_description += new_description;
			kadu->setStatus(sess->status);
			length_buffor=new_description.length();
			//}
		}
	start(1000, TRUE);
}

AutoAwayTimer::AutoAwayTimer(QObject* parent) : QTimer(parent,"AutoAwayTimer") {
	autoawayed = false;
	a->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(onTimeout()));
	start(config.autoawaytime * 1000,TRUE);
}

bool AutoAwayTimer::eventFilter(QObject *o,QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter) {
		stop();
		start(config.autoawaytime * 1000, TRUE);
		if (autoawayed) {
			fprintf(stderr, "KK AutoAwayTimer::eventFilter(type = QEvent::KeyPress or QEvent::Enter): auto away cancelled\n");
			autoawayed = false;
			kadu->setStatus(beforeAutoAway);
			}
		}
	return QObject::eventFilter(o, e);
}

void AutoAwayTimer::onTimeout()
{
	if (!autoawayed) {
		beforeAutoAway = getActualStatus() & (~GG_STATUS_FRIENDS_MASK);;
		fprintf(stderr, "KK AutoAwayTimer::onTimeout(): checking whether to go auto away, beforeAutoAway = %d\n", beforeAutoAway);
		switch (beforeAutoAway) {
			case GG_STATUS_AVAIL_DESCR: kadu->setStatus(GG_STATUS_BUSY_DESCR); break;
			case GG_STATUS_AVAIL: kadu->setStatus(GG_STATUS_BUSY); break;
			default: start(config.autoawaytime * 1000, TRUE); return;
			}
		fprintf(stderr, "KK AutoAwayTimer::onTimeout(): I am away!\n");
		autoawayed = true;
	}
//potrzebne na wypadek zerwania polaczenia i ponownego polaczenia sie z statusem innym niz busy*
	start(config.autoawaytime * 1000, TRUE);
}

#include "status.moc"
