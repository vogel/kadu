/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qdesktopwidget.h>

#include "status.h"
#include "kadu.h"
#include "debug.h"

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
		QFile f(ggPath("description"));
		if (!f.open(IO_ReadOnly)) {
			start(1000, TRUE);
			return;
			}
		QTextStream s(&f);
		QString new_description;
		new_description = s.readLine();
		f.close();
		f.remove(ggPath("description"));
		//if (new_description != own_description) {
			kdebug("AutoStatus: adding \"%s\" to description\n", new_description.local8Bit().data());
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
//	QApplication::desktop()->installEventFilter(this);
//	QApplication::desktop()->grabMouse();
	a->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(onTimeout()));
	start(config.autoawaytime * 1000,TRUE);
}

bool AutoAwayTimer::eventFilter(QObject *o,QEvent *e)
{
//	kdebug("AutoAwayTimer::eventFilter()\n");
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove) {
		stop();
		start(config.autoawaytime * 1000, TRUE);
		if (autoawayed) {
			kdebug("AutoAwayTimer::eventFilter(type = QEvent::KeyPress or QEvent::Enter): auto away cancelled\n");
			autoawayed = false;
			kadu->setStatus(beforeAutoAway);
//			QApplication::desktop()->releaseMouse();
//			QApplication::desktop()->releaseKeyboard();
//			a->sendEvent(o, e);
			}
		}
	return QObject::eventFilter(o, e);
}

void AutoAwayTimer::onTimeout()
{
	if (!autoawayed) {
		beforeAutoAway = getActualStatus() & (~GG_STATUS_FRIENDS_MASK);;
		kdebug("AutoAwayTimer::onTimeout(): checking whether to go auto away, beforeAutoAway = %d\n", beforeAutoAway);
		switch (beforeAutoAway) {
			case GG_STATUS_AVAIL_DESCR:
				kadu->setStatus(GG_STATUS_BUSY_DESCR);
//				QApplication::desktop()->grabMouse();
//				QApplication::desktop()->grabKeyboard();
				autoawayed = true;
				break;
			case GG_STATUS_AVAIL:
				kadu->setStatus(GG_STATUS_BUSY);
//				QApplication::desktop()->grabMouse();
//				QApplication::desktop()->grabKeyboard();
				autoawayed = true;
				break;
			default:
				start(config.autoawaytime * 1000, TRUE);
				return;
			}
		kdebug("AutoAwayTimer::onTimeout(): I am away!\n");
		}
//potrzebne na wypadek zerwania polaczenia i ponownego polaczenia sie z statusem innym niz busy*
	start(config.autoawaytime * 1000, TRUE);
}

void AutoAwayTimer::on() {
	if (autoaway_object == NULL) {
		autoaway_object = new AutoAwayTimer();
		autoaway_object->stop();
		autoaway_object->start(config.autoawaytime * 1000, TRUE);
		}
}

void AutoAwayTimer::off(){
	if (autoaway_object != NULL){
		delete autoaway_object;
		autoaway_object = NULL;
		}
}
