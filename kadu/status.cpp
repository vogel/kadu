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

#include "gadu.h"
#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "kadu.h"

#include "status.h"

AutoAwayTimer* AutoAwayTimer::autoaway_object=NULL;

QString gg_icons[] = {"Online", "OnlineWithDescription", "Busy", "BusyWithDescription", "Invisible", "InvisibleWithDescription",
	"Offline", "OfflineWithDescription", "Blocking"};

int gg_statuses[] = {GG_STATUS_AVAIL, GG_STATUS_AVAIL_DESCR, GG_STATUS_BUSY, GG_STATUS_BUSY_DESCR,
	GG_STATUS_INVISIBLE, GG_STATUS_INVISIBLE_DESCR, GG_STATUS_NOT_AVAIL, GG_STATUS_NOT_AVAIL_DESCR,
	GG_STATUS_BLOCKED};

const char *statustext[] = {
	QT_TR_NOOP("Online"),
	QT_TR_NOOP("Online (d.)"),
	QT_TR_NOOP("Busy"),
	QT_TR_NOOP("Busy (d.)"),
	QT_TR_NOOP("Invisible"),
	QT_TR_NOOP("Invisible (d.)"),
	QT_TR_NOOP("Offline"),
	QT_TR_NOOP("Offline (d.)"),
	QT_TR_NOOP("Blocking")
};

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

bool isAvailableStatus(unsigned int status)
{
	return (status != GG_STATUS_NOT_AVAIL && status != GG_STATUS_NOT_AVAIL_DESCR);
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

AutoAwayTimer::AutoAwayTimer(QObject* parent) : QTimer(parent,"AutoAwayTimer"), idletime(0) {
	autoawayed = false;
	qApp->installEventFilter(this);
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
	if (idletime >= config_file.readNumEntry("General","AutoAwayTime") && !autoawayed) {
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
		if (idletime < config_file.readNumEntry("General","AutoAwayTime") && autoawayed) {
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
	QT_TRANSLATE_NOOP("@default", "General");
	QT_TRANSLATE_NOOP("@default", "Enable autoaway");
	QT_TRANSLATE_NOOP("@default", "Set status to away after ");
	QT_TRANSLATE_NOOP("@default", " seconds");
	QT_TRANSLATE_NOOP("@default", "Default status");
	QT_TRANSLATE_NOOP("@default", "On shutdown, set description:");
	QT_TRANSLATE_NOOP("@default", "Enable dock icon");
	QT_TRANSLATE_NOOP("@default", "Start docked");
	QT_TRANSLATE_NOOP("@default", "Private status");
	QT_TRANSLATE_NOOP("@default", "Check for updates");
	QT_TRANSLATE_NOOP("@default", "Add to description");


	kdebug("AutoAwayTimer::initModule() \n");
	ConfigDialog::addTab("General");
	ConfigDialog::addVGroupBox("General", "General", "Status");
	ConfigDialog::addCheckBox("General", "Status", "Enable autoaway", "AutoAway", false);
	ConfigDialog::addHBox("General", "Status", "--");
	ConfigDialog::addLineEdit("General", "--", "Set status to away after ", "AutoAwayTime", "300");
	ConfigDialog::addLabel("General", "--", " seconds");
	ConfigDialog::addComboBox("General", "Status", "Default status", "", "cb_defstatus");
	ConfigDialog::addHBox("General", "Status", "discstatus");
	ConfigDialog::addCheckBox("General", "discstatus", "On shutdown, set description:", "DisconnectWithDescription", false);
	ConfigDialog::addLineEdit("General", "discstatus", "", "DisconnectDescription", "", "", "e_defaultstatus");
	AutoAwaySlots *autoawayslots= new AutoAwaySlots();
	ConfigDialog::registerSlotOnCreate(autoawayslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(autoawayslots, SLOT(onDestroyConfigDialog()));
	
	ConfigDialog::addGrid("General", "General", "grid", 3);
	ConfigDialog::addCheckBox("General", "grid", "Enable dock icon", "UseDocking", true);
	ConfigDialog::addCheckBox("General", "grid", "Start docked", "RunDocked", false);
	ConfigDialog::addCheckBox("General", "grid", "Private status", "PrivateStatus", false);
	ConfigDialog::addCheckBox("General", "grid", "Check for updates", "CheckUpdates", true);
};

void AutoAwaySlots::onCreateConfigDialog()
{
	kdebug("AutoAwayTimer::onCreateConfigDialog() \n");
	QHBox *awygrp = ConfigDialog::getHBox("General", "--");
	QCheckBox * b_autoaway= ConfigDialog::getCheckBox("General", "Enable autoaway");
	awygrp->setEnabled(b_autoaway->isChecked());
	connect(b_autoaway,SIGNAL(toggled(bool)),awygrp,SLOT(setEnabled(bool)));
	
	QCheckBox *b_disconnectdesc= ConfigDialog::getCheckBox("General", "On shutdown, set description:");
	QLineEdit *e_disconnectdesc= ConfigDialog::getLineEdit("General", "", "e_defaultstatus");
	e_disconnectdesc->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	e_disconnectdesc->setEnabled(b_disconnectdesc->isChecked());
	connect(b_disconnectdesc, SIGNAL(toggled(bool)), e_disconnectdesc, SLOT(setEnabled(bool)));

	QComboBox* cb_defstatus= ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	int statusnr=config_file.readNumEntry("General", "DefaultStatus", GG_STATUS_NOT_AVAIL);
	cb_defstatus->clear();
	int i;
	for (i = 0;i < 7; i++)
		cb_defstatus->insertItem(qApp->translate("@default", statustext[i]));
	i=0;	
	while (i<7 && statusnr !=gg_statuses[i])
		i++;
	cb_defstatus->setCurrentItem(i);
	
}

void AutoAwaySlots::onDestroyConfigDialog()
{
	kdebug("AutoAwayTimer::onDestroyConfigDialog() \n");
	QComboBox* cb_defstatus= ConfigDialog::getComboBox("General", "Default status", "cb_defstatus");
	config_file.writeEntry("General", "DefaultStatus", gg_statuses[cb_defstatus->currentItem()]);

	if (config_file.readBoolEntry("General", "AutoAway"))
	        AutoAwayTimer::on();
	else
                AutoAwayTimer::off();

	int status = getActualStatus();

	if (status != GG_STATUS_NOT_AVAIL)
	if ((!(status & GG_STATUS_FRIENDS_MASK)&& 
	    config_file.readBoolEntry("General", "PrivateStatus"))
	|| ((status & GG_STATUS_FRIENDS_MASK) &&  
	    !config_file.readBoolEntry("General", "PrivateStatus")))
	    kadu->setStatus(status & (~GG_STATUS_FRIENDS_MASK));
	
	statusppm->setItemChecked(8, config_file.readBoolEntry("General", "PrivateStatus"));

};

