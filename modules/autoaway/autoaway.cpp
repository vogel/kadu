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
#include <qcursor.h> 

#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "kadu.h"
#include "events.h"

#include "autoaway.h"

AutoAwayTimer *autoaway_object=NULL;
AutoAwaySlots *autoawayslots=NULL;

extern "C" int autoaway_init()
{
	kdebugf();
	
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"));
	ConfigDialog::addVGroupBox("General", "General", "Status");
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Enable autoaway"), "AutoAway", false);
	ConfigDialog::addHBox("General", "Status", "times");
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to away after "), "AutoAwayTime", 1, 10000, 1, 300);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Check idle every "), "AutoAwayCheckTime", 1, 10000, 1, 1);

	autoawayslots= new AutoAwaySlots();
	ConfigDialog::registerSlotOnCreate(autoawayslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(autoawayslots, SLOT(onApplyConfigDialog()));
	
	QObject::connect(kadu, SIGNAL(disconnectingNetwork()), autoawayslots, SLOT(off()));
	QObject::connect(&event_manager, SIGNAL(connected()), autoawayslots, SLOT(on()));
	return 0;
}

extern "C" void autoaway_close()
{
	kdebugf();
	autoawayslots->off();
	QObject::disconnect(kadu, SIGNAL(disconnectingNetwork()), autoawayslots, SLOT(off()));
	QObject::disconnect(&event_manager, SIGNAL(connected()), autoawayslots, SLOT(on()));
	ConfigDialog::unregisterSlotOnCreate(autoawayslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(autoawayslots, SLOT(onApplyConfigDialog()));
	delete autoawayslots;
	autoawayslots=NULL;
	ConfigDialog::removeControl("General", "Check idle every ");
	ConfigDialog::removeControl("General", "Set status to away after ");
	ConfigDialog::removeControl("General", "times");
	ConfigDialog::removeControl("General", "Enable autoaway");
	ConfigDialog::removeControl("General", "Status");
}

AutoAwayTimer::AutoAwayTimer(QObject* parent) : QTimer(parent,"AutoAwayTimer"), idletime(0) {
	autoawayed = false;
	qApp->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(checkIdleTime()));
	start(config_file.readNumEntry("General", "AutoAwayCheckTime")*1000, TRUE);
}

// jesli wciskamy klawisze lub poruszamy myszka w obrebie programu to zerujemy czas nieaktywnosci
bool AutoAwayTimer::eventFilter(QObject *o,QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove)
		idletime = 0;
	return QObject::eventFilter(o, e);
}

//metoda wywo³ywana co sekundê(mo¿liwa zmiana w konfiguracji) w celu sprawdzenia czy mamy staæ siê "zajêci"
void AutoAwayTimer::checkIdleTime()
{
	static int kbdirqs = 0;
	static int mouseirqs = 0;
	static int i8042irqs = 0;
	static QPoint mousepos(0, 0);
	
	int actkbdirqs   = 0;
	int actmouseirqs = 0;
	int acti8042irqs = 0;
	bool inactive = true;
	QPoint actmousepos;

	actmousepos=QCursor::pos();
	if(actmousepos==mousepos)
		inactive = true;
	else
		idletime = 0;

	mousepos=actmousepos;

	int autoAwayTime=config_file.readNumEntry("General","AutoAwayTime");
	int autoAwayCheckTime=config_file.readNumEntry("General","AutoAwayCheckTime");
//	sprawdzenie czy wzrosla liczba obsluzonych przerwan klawiatury lub myszki
	QFile f("/proc/interrupts");
	if (f.open(IO_ReadOnly)) {
		QTextStream stream(&f);
		QString line;
		QStringList strlist;
		while (!stream.atEnd() && (line = stream.readLine()) != QString::null) {
			if (line.contains(QRegExp("i8042"))) {
				strlist = QStringList::split(" ", line);
				acti8042irqs += strlist[1].toUInt();
				}
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
		if (actkbdirqs == kbdirqs && actmouseirqs == mouseirqs && acti8042irqs == i8042irqs)
			inactive=true;
		else
			idletime = 0;
		kbdirqs = actkbdirqs;
		mouseirqs = actmouseirqs;
		i8042irqs = acti8042irqs;
		}
	
	if(inactive)
		idletime+=autoAwayCheckTime;

//	czy mamy stac sie "zajeci" po config.autoawaytime sekund nieaktywnosci
	if (idletime >= autoAwayTime && !autoawayed) {
		beforeAutoAway = getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK);;
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
				start(autoAwayCheckTime*1000, TRUE);
				return;
			}
		kdebug("AutoAwayTimer::checkIdleTime(): I am away!\n");
		}
	else
//		jesli bylismy "zajeci" to stajemy sie z powrotem "dostepni"
		if (idletime < autoAwayTime && autoawayed) {
			kdebug("AutoAwayTimer::checkIdleTime(): auto away cancelled\n");
			autoawayed = false;
			kadu->setStatus(beforeAutoAway);
			}

//potrzebne na wypadek zerwania polaczenia i ponownego polaczenia sie z statusem innym niz busy*
	start(autoAwayCheckTime*1000, TRUE);
}

void AutoAwaySlots::on() {
	if (!autoaway_object && config_file.readBoolEntry("General", "AutoAway"))
		autoaway_object = new AutoAwayTimer();
}

void AutoAwaySlots::off() {
	if (autoaway_object && config_file.readBoolEntry("General", "AutoAway"))
	{
		delete autoaway_object;
		autoaway_object = NULL;
	}
}

void AutoAwaySlots::onCreateConfigDialog()
{
	kdebugf();
	QHBox *awygrp = ConfigDialog::getHBox("General", "times");
	QCheckBox * b_autoaway= ConfigDialog::getCheckBox("General", "Enable autoaway");
	awygrp->setEnabled(b_autoaway->isChecked());
	connect(b_autoaway,SIGNAL(toggled(bool)),awygrp,SLOT(setEnabled(bool)));
	
	ConfigDialog::getSpinBox("General", "Set status to away after ")->setSuffix(" s");
	ConfigDialog::getSpinBox("General", "Check idle every ")->setSuffix(" s");
}

void AutoAwaySlots::onApplyConfigDialog()
{
	kdebugf();

	if (config_file.readBoolEntry("General", "AutoAway"))
		on();
	else
		off();
}
