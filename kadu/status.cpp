/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qhgroupbox.h>

#include "config_file.h"
#include "config_dialog.h"
#include "kadu.h"
#include "debug.h"

AutoAwayTimer* AutoAwayTimer::autoaway_object=NULL;

QString gg_icons[] = {"online", "online_d", "busy", "busy_d", "invisible", "invisible_d",
	"offline", "offline_d", "blocking"};

int gg_statuses[] = {GG_STATUS_AVAIL, GG_STATUS_AVAIL_DESCR, GG_STATUS_BUSY, GG_STATUS_BUSY_DESCR,
	GG_STATUS_INVISIBLE, GG_STATUS_INVISIBLE_DESCR, GG_STATUS_NOT_AVAIL, GG_STATUS_NOT_AVAIL_DESCR,
	GG_STATUS_BLOCKED};

const char *statustext[] = {"Online", "Online (d.)",
	"Busy", "Busy (d.)",
	"Invisible", "Invisible (d.)",
	"Offline", "Offline (d.)", "Blocking"};

/* our own description container */
QString own_description;
QStringList defaultdescriptions;

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
	if (sess && ifStatusWithDescription(sess->status) && config_file.readBoolEntry("Global","AddToDescription")) {
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

AutoAwayTimer::AutoAwayTimer(QObject* parent) : QTimer(parent,"AutoAwayTimer"), idletime(0) {
	autoawayed = false;
	a->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(checkIdleTime()));
	start(1000, TRUE);
}

// jesli wciskamy klawisze lub poruszamy myszka w obrebie programu to zerujemy czas nieaktywnosci
bool AutoAwayTimer::eventFilter(QObject *o,QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove)
		idletime = 0;
	return QObject::eventFilter(o, e);
}

//metoda wywolywana co sekunde w celu sprawdzenia czy mamy stac sie "zajeci"
void AutoAwayTimer::checkIdleTime()
{
	static int kbdirqs = 0;
	static int mouseirqs = 0;
	int actkbdirqs, actmouseirqs;

//	sprawdzenie czy wzrosla liczba obsluzonych przerwan klawiatury lub myszki
	QFile f("/proc/interrupts");
	if (f.open(IO_ReadOnly)) {
		QTextStream stream(&f);
		QString line;
		QStringList strlist;
		while (!stream.atEnd() && (line = stream.readLine()) != QString::null) {
			if (line.contains(QRegExp("keyboard"))) {
				strlist = QStringList::split(" ", line);
				actkbdirqs = strlist[1].toUInt();
				}
			if (line.contains(QRegExp("(M|m)ouse"))) {
				strlist = QStringList::split(" ", line);
				actmouseirqs = strlist[1].toUInt();
				}
			}
		f.close();
		if (actkbdirqs == kbdirqs && actmouseirqs == mouseirqs)
			idletime++;
		else
			idletime = 0;
		kbdirqs = actkbdirqs;
		mouseirqs = actmouseirqs;
		}
	else
		idletime++;

//	czy mamy stac sie "zajeci" po config.autoawaytime sekund nieaktywnosci
	if (idletime >= config_file.readNumEntry("Global","AutoAwayTime") && !autoawayed) {
		beforeAutoAway = getActualStatus() & (~GG_STATUS_FRIENDS_MASK);;
		kdebug("AutoAwayTimer::checkIdleTime(): checking whether to go auto away, beforeAutoAway = %d\n", beforeAutoAway);
		switch (beforeAutoAway) {
			case GG_STATUS_AVAIL_DESCR:
				kadu->setStatus(GG_STATUS_BUSY_DESCR);
				autoawayed = true;
				break;
			case GG_STATUS_AVAIL:
				kadu->setStatus(GG_STATUS_BUSY);
				autoawayed = true;
				break;
			default:
				start(1000, TRUE);
				return;
			}
		kdebug("AutoAwayTimer::checkIdleTime(): I am away!\n");
		}
	else
//		jesli bylismy "zajeci" to stajemy sie z powrotem "dostepni"
		if (idletime < config_file.readNumEntry("Global","AutoAwayTime") && autoawayed) {
			kdebug("AutoAwayTimer::checkIdleTime(): auto away cancelled\n");
			autoawayed = false;
			kadu->setStatus(beforeAutoAway);
			}

//potrzebne na wypadek zerwania polaczenia i ponownego polaczenia sie z statusem innym niz busy*
	start(1000, TRUE);
}

void AutoAwayTimer::on() {
	if (!autoaway_object)
		autoaway_object = new AutoAwayTimer();
}

void AutoAwayTimer::off() {
	if (autoaway_object) {
		delete autoaway_object;
		autoaway_object = NULL;
		}
}

void AutoAwayTimer::initModule()
{
	kdebug("AutoAwayTimer::initModule() \n");
	ConfigDialog::registerTab(i18n("General"));
	ConfigDialog::registerCheckBox(i18n("General"),i18n("Enable autoaway"),"Global","AutoAway",false);
	ConfigDialog::registerHGroupBox(i18n("General"),"--");
	ConfigDialog::registerLineEdit("--",i18n("Set status to away after "),"Global","AutoAwayTime","300");
	ConfigDialog::registerLabel("--",i18n(" seconds"));	
	ConfigDialog::registerHGroupBox(i18n("General"),i18n("Default Status"));
	ConfigDialog::registerComboBox(i18n("Default Status"),"","Global","DefaultStatus","","cb_defstatus");
	ConfigDialog::registerCheckBox(i18n("General"),i18n("On shutdown, set description:"),"Other","DisconnectWithDescription",false);
	ConfigDialog::registerLineEdit(i18n("General"),"","Other","DisconnectDescription","","","e_defaultstatus");	
	AutoAwaySlots *autoawayslots=new AutoAwaySlots();
	ConfigDialog::registerSlotOnCreate(autoawayslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(autoawayslots,SLOT(onDestroyConfigDialog()));
}

void AutoAwaySlots::onCreateConfigDialog()
{
	kdebug("AutoAwayTimer::onCreateConfigDialog() \n");
	QHGroupBox *awygrp = (QHGroupBox*)(ConfigDialog::getWidget(i18n("General"),"--"));
	QCheckBox * b_autoaway= (QCheckBox*)(ConfigDialog::getWidget(i18n("General"),i18n("Enable autoaway")));
	awygrp->setEnabled(b_autoaway->isChecked());
	connect(b_autoaway,SIGNAL(toggled(bool)),awygrp,SLOT(setEnabled(bool)));
	
	QCheckBox *b_disconnectdesc=(QCheckBox*)(ConfigDialog::getWidget(i18n("General"),i18n("On shutdown, set description:")));
	QLineEdit *e_disconnectdesc=(QLineEdit*)(ConfigDialog::getWidget(i18n("General"),"","e_defaultstatus"));
	e_disconnectdesc->setEnabled(b_disconnectdesc->isChecked());
	connect(b_disconnectdesc,SIGNAL(toggled(bool)),e_disconnectdesc,SLOT(setEnabled(bool)));

	QComboBox* cb_defstatus=(QComboBox*)(ConfigDialog::getWidget(i18n("Default Status"),"","cb_defstatus"));
	int statusnr=config_file.readNumEntry("Global","DefaultStatus",GG_STATUS_NOT_AVAIL);
	cb_defstatus->clear();
	int i;
	for (i = 0;i < 7; i++)
		cb_defstatus->insertItem(i18n(__c2q(statustext[i])));
	i=0;	
	while (i<7 && statusnr !=gg_statuses[i])
		i++;
	cb_defstatus->setCurrentItem(i);
	
}

void AutoAwaySlots::onDestroyConfigDialog()
{
	kdebug("AutoAwayTimer::onDestroyConfigDialog() \n");
	QComboBox* cb_defstatus=(QComboBox*)(ConfigDialog::getWidget(i18n("Default Status"),"","cb_defstatus"));
	config_file.writeEntry("Global","DefaultStatus",gg_statuses[cb_defstatus->currentItem()]);
	config_file.sync();	
}
