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

#include "message_box.h"
#include "autoaway.h"

AutoAwayTimer *autoaway_object=NULL;
AutoAwaySlots *autoawayslots=NULL;

extern "C" int autoaway_init()
{
	kdebugf();
	
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"));

	ConfigDialog::addHGroupBox("General", "General", "AutoStatus");
	ConfigDialog::addHBox("General", "AutoStatus", "autoStatus");
	ConfigDialog::addVBox("General", "autoStatus", "enables");
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autoaway"), "AutoAway", false);
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autoinvisible"), "AutoInvisible", false);
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autodisconnect"), "AutoDisconnect", false);
	ConfigDialog::addVBox("General", "autoStatus", "times");
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to away after "), "AutoAwayTime", 1, 10000, 1, 180);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to invisible after "), "AutoInvisibleTime", 1, 10000, 1, 600);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Disconnect after "), "AutoDisconnectTime", 1, 10000, 1, 1800);
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Enable AutoStatus"), "AutoChange", false);
	ConfigDialog::addSpinBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Check idle every "), "AutoAwayCheckTime", 1, 10000, 1, 1);
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Autorestore status"), "AutoRestoreStatus", true);
	ConfigDialog::addHGroupBox("General", "General", "AutoStatus Description");
	ConfigDialog::addHBox("General", "AutoStatus Description", "first");
	ConfigDialog::addLineEdit("General", "first",
			QT_TRANSLATE_NOOP("@default", "Auto change status"), "AutoStatusText", "");
	ConfigDialog::addHBox("General", "AutoStatus Description", "second");	
	ConfigDialog::addLabel("General", "second", "0");
	ConfigDialog::addComboBox("General", "second",  " ");
		
	autoawayslots= new AutoAwaySlots();
	ConfigDialog::registerSlotOnCreate(autoawayslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(autoawayslots, SLOT(onApplyConfigDialog()));
	
	QObject::connect(kadu, SIGNAL(disconnectingNetwork()), autoawayslots, SLOT(off()));
	QObject::connect(gadu, SIGNAL(connected()), autoawayslots, SLOT(on()));
	kdebugf2();
	return 0;
}

extern "C" void autoaway_close()
{
	kdebugf();
	autoawayslots->off();
	QObject::disconnect(kadu, SIGNAL(disconnectingNetwork()), autoawayslots, SLOT(off()));
	QObject::disconnect(gadu, SIGNAL(connected()), autoawayslots, SLOT(on()));
	ConfigDialog::unregisterSlotOnCreate(autoawayslots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(autoawayslots, SLOT(onApplyConfigDialog()));
	delete autoawayslots;
	autoawayslots=NULL;
	ConfigDialog::removeControl("General", "Check idle every ");
	ConfigDialog::removeControl("General", "Enable AutoStatus");
	ConfigDialog::removeControl("General", "Set status to away after ");
	ConfigDialog::removeControl("General", "Set status to invisible after ");
	ConfigDialog::removeControl("General", "Disconnect after ");
	ConfigDialog::removeControl("General", "times");
	ConfigDialog::removeControl("General", "Enable autoaway");
	ConfigDialog::removeControl("General", "Enable autoinvisible");
	ConfigDialog::removeControl("General", "Enable autodisconnect");
	ConfigDialog::removeControl("General", "enables");
	ConfigDialog::removeControl("General", "autoStatus");
	ConfigDialog::removeControl("General", "AutoStatus");
	ConfigDialog::removeControl("General","Autorestore status");
	ConfigDialog::removeControl("General", "Auto change status");
	ConfigDialog::removeControl("General", " ");
	ConfigDialog::removeControl("General", "0");
	ConfigDialog::removeControl("General", "first");
	ConfigDialog::removeControl("General", "second");
	ConfigDialog::removeControl("General", "AutoStatus Description");
	
	kdebugf2();
}

AutoAwayTimer::AutoAwayTimer(QObject* parent) : QTimer(parent,"AutoAwayTimer"), idletime(0) {
	autoawayed = false;
	autoinvisibled=false;
	autodisconnected=false;
	autodescription=false;
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
	int autoDisconnectTime=config_file.readNumEntry("General","AutoDisconnectTime");
	int autoInvisibleTime=config_file.readNumEntry("General","AutoInvisibleTime");
	int autoAwayCheckTime=config_file.readNumEntry("General","AutoAwayCheckTime");
	bool autoAway=config_file.readBoolEntry("General","AutoAway");
	bool autoInvisible=config_file.readBoolEntry("General","AutoInvisible");
	bool autoDisconnect=config_file.readBoolEntry("General","AutoDisconnect");
	bool autoRestoreStatus=config_file.readBoolEntry("General","AutoRestoreStatus");
	int autoChangeDescription=config_file.readNumEntry("General", "AutoChangeDescription");
	QString autoStatusText=config_file.readEntry("General", "AutoStatusText");
	
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
	
	int currentStatus = gadu->getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK); /* to sie przyda pozniej */

	/* czy sie nie rozlaczyc */
	//if (idletime >= autoDisconnectTime && autoDisconnect && ((currentStatus==GG_STATUS_INVISIBLE_DESCR) || (currentStatus==GG_STATUS_INVISIBLE)) && !autodisconnected)
	if (idletime >= autoDisconnectTime && autoDisconnect && !autodisconnected)
	{	kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): checking whether to disconnect, beforeAutoDisconnect = %d\n", currentStatus);
			if (!autoinvisibled)
				{	beforeAutoAway = gadu->getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK);
					autoinvisibled=true;
					autoawayed=true;
				}	
			if (!autodescription)
			   if (autoChangeDescription==1)
					{	oldStatus=own_description;
						own_description=autoStatusText;
						autodescription=true;
					}
				else if ((autoChangeDescription==2) || (autoChangeDescription==3))
					{	oldStatus=own_description;
						QString new_description;
						if (autoChangeDescription==2) new_description=autoStatusText+own_description;
							else 
								{	own_description.truncate(70-autoStatusText.length()-1);
									new_description=own_description+autoStatusText;
								}	
						new_description.truncate(70);
						own_description=new_description;
						autodescription=true;
					}
			gadu->logout();
			autodisconnected=true;
			kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): I am disconnected!\n");
		}
	else
	/* potem sprawdzamy czy nie idziemy do ukrytego - ale to tylko jak wczesniej byl busy */
	//if (idletime >= autoInvisibleTime && autoInvisible && ((currentStatus==GG_STATUS_BUSY_DESCR) || (currentStatus==GG_STATUS_BUSY)) && !autoinvisibled && !autodisconnected)
	if (idletime >= autoInvisibleTime && autoInvisible && !autoinvisibled)
		{	kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): checking whether to go invisible, beforeAutoInvisible = %d\n", currentStatus);
			if (!autoawayed) 
				{	beforeAutoAway = gadu->getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK);
					autoawayed=true;
				}
			if (!autodescription)
			    if (autoChangeDescription==1)
					{	oldStatus=own_description;
						own_description=autoStatusText;
						autodescription=true;
					}
				else if ((autoChangeDescription==2) || (autoChangeDescription==3))
					{	oldStatus=own_description;
						QString new_description;
						if (autoChangeDescription==2) new_description=autoStatusText+own_description;
							else 
								{	own_description.truncate(70-autoStatusText.length());
									new_description=own_description+autoStatusText;
								}	
						new_description.truncate(70);
						own_description=new_description;
						autodescription=true;
					}
			switch (currentStatus)
			{	case GG_STATUS_BUSY_DESCR:
					kadu->setStatus(GG_STATUS_INVISIBLE_DESCR);		
					autoinvisibled=true;
					break;
				case GG_STATUS_BUSY:
					kadu->setStatus(GG_STATUS_INVISIBLE);
					autoinvisibled=true;
					break;
				case GG_STATUS_AVAIL_DESCR:
					kadu->setStatus(GG_STATUS_INVISIBLE_DESCR);
					autoinvisibled=true;
					break;
				case GG_STATUS_AVAIL:
					kadu->setStatus(GG_STATUS_INVISIBLE);
					autoinvisibled=true;
					break;
				default:
					start(autoAwayCheckTime*1000, TRUE);
					return;
			}
			kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): I am invisible!\n");
		}
	else 
	//	czy mamy stac sie "zajeci" po config.autoawaytime sekund nieaktywnosci
	if (idletime >= autoAwayTime && !autoawayed && autoAway) {
		beforeAutoAway = gadu->getCurrentStatus() & (~GG_STATUS_FRIENDS_MASK);
		kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): checking whether to go auto away, beforeAutoAway = %d\n", beforeAutoAway);
		if (!autodescription)
			    if (autoChangeDescription==1)
					{	oldStatus=own_description;
						own_description=autoStatusText;
						autodescription=true;
					}
				else if ((autoChangeDescription==2) || (autoChangeDescription==3))
					{	oldStatus=own_description;
						QString new_description;
						if (autoChangeDescription==2) new_description=autoStatusText+own_description;
							else 
								{	own_description.truncate(70-autoStatusText.length());
									new_description=own_description+autoStatusText;
								}	
						new_description.truncate(70);
						own_description=new_description;
						autodescription=true;
					}
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
		kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): I am away!\n");
	} 
	else
//		jesli bylismy "zajeci" to stajemy sie z powrotem "dostepni" jasli nie mamy tego zablokowanego
	if (idletime < autoAwayTime && autoawayed && autoRestoreStatus)
		{	kdebugm(KDEBUG_INFO, "AutoAwayTimer::checkIdleTime(): auto away cancelled\n");
			
			autodisconnected=false;
			autoinvisibled=false;
			autoawayed = false;
			if (autodescription)  own_description=oldStatus;
			autodescription=false;
			
			kadu->setStatus(beforeAutoAway);
		}

//potrzebne na wypadek zerwania polaczenia i ponownego polaczenia sie z statusem innym niz busy*
	start(autoAwayCheckTime*1000, TRUE);
}

void AutoAwaySlots::on() {
	if (!autoaway_object && config_file.readBoolEntry("General", "AutoChange"))
		autoaway_object = new AutoAwayTimer();
}

void AutoAwaySlots::off() {
	if (autoaway_object && config_file.readBoolEntry("General", "AutoChange"))
	{
		delete autoaway_object;
		autoaway_object = NULL;
	}
}

void AutoAwaySlots::onCreateConfigDialog()
{
	kdebugf();
		
	QHBox *awygrp = ConfigDialog::getHBox("General", "times");
	QHBox *awygrp2 = ConfigDialog::getHBox("General", "enables");
	QHGroupBox *awygrp3 = ConfigDialog::getHGroupBox("General", "AutoStatus Description");
	QCheckBox *b_autostatus= ConfigDialog::getCheckBox("General", "Enable AutoStatus");
	QCheckBox *b_autoaway= ConfigDialog::getCheckBox("General", "Enable autoaway");
	QCheckBox *b_autoinvisible= ConfigDialog::getCheckBox("General", "Enable autoinvisible");
	QCheckBox *b_autodisconnect= ConfigDialog::getCheckBox("General", "Enable autodisconnect");
	QCheckBox *b_autorestore= ConfigDialog::getCheckBox("General", "Autorestore status");
	/* wylaczenie AutoStatus wyszarza wszystko */
	awygrp->setEnabled(b_autostatus->isChecked());
	awygrp2->setEnabled(b_autostatus->isChecked());
	awygrp3->setEnabled(b_autostatus->isChecked());
	b_autorestore->setEnabled(b_autostatus->isChecked());
	QSpinBox *autoawayTime= ConfigDialog::getSpinBox("General", "Check idle every ");
	autoawayTime->setEnabled(b_autostatus->isChecked());
	/* wyszarzanie SpinBoxow przy wlaczonym AutoStatus */
	QSpinBox *autoawaySpin= ConfigDialog::getSpinBox("General", "Set status to away after ");
	autoawaySpin->setEnabled(b_autoaway->isChecked());
	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	invisibleSpin->setEnabled(b_autoinvisible->isChecked());
	QSpinBox *disconnectSpin= ConfigDialog::getSpinBox("General", "Disconnect after ");
	disconnectSpin->setEnabled(b_autodisconnect->isChecked());	
	/* podpinanie sie do slotow zmiany dostepnosci */
	connect(b_autostatus, SIGNAL(toggled(bool)), awygrp, SLOT(setEnabled(bool)));
	connect(b_autostatus, SIGNAL(toggled(bool)), awygrp2, SLOT(setEnabled(bool)));
	connect(b_autostatus, SIGNAL(toggled(bool)), awygrp3, SLOT(setEnabled(bool)));
	connect(b_autostatus, SIGNAL(toggled(bool)), b_autorestore, SLOT(setEnabled(bool)));
	connect(b_autostatus, SIGNAL(toggled(bool)), autoawayTime, SLOT(setEnabled(bool)));
	connect(b_autoaway, SIGNAL(toggled(bool)), autoawaySpin, SLOT(setEnabled(bool)));
	connect(b_autoinvisible, SIGNAL(toggled(bool)), invisibleSpin, SLOT(setEnabled(bool)));
	connect(b_autodisconnect, SIGNAL(toggled(bool)), disconnectSpin, SLOT(setEnabled(bool)));
	/* podpinanie sie pod zmiane SpinBoxow */
	connect(autoawaySpin, SIGNAL(valueChanged(int)), this, SLOT(changeAutoInvisibleTime(int)));
	connect(invisibleSpin, SIGNAL(valueChanged(int)), this, SLOT(changeAutoDisconnectTime(int)));
	connect(disconnectSpin, SIGNAL(valueChanged(int)), this, SLOT(correctAutoDisconnectTime(int)));
	/* jeszcze jedno podpinanie pod checkboxy */
	connect(b_autoaway, SIGNAL(toggled(bool)), this, SLOT(checkAutoInvisibleTime(bool)));
	connect(b_autoinvisible, SIGNAL(toggled(bool)), this, SLOT(checkAutoDisconnectTime(bool)));
	/* tylko czy to zadziala :P */
	ConfigDialog::getSpinBox("General", "Set status to away after ")->setSuffix(" s");
	ConfigDialog::getSpinBox("General", "Set status to invisible after ")->setSuffix(" s");
	ConfigDialog::getSpinBox("General", "Disconnect after ")->setSuffix(" s");
	ConfigDialog::getSpinBox("General", "Check idle every ")->setSuffix(" s");
	
	QComboBox *autoDescription= ConfigDialog::getComboBox("General"," ");
	autoDescription->insertItem("Don't change the description");
	autoDescription->insertItem("Change");
	autoDescription->insertItem("Add in front of description");
	autoDescription->insertItem("Add at the back of description");
	
	int i=config_file.readNumEntry("General", "AutoChangeDescription");
	autoDescription->setCurrentItem(i);
	
	QLineEdit *autoStatusText=ConfigDialog::getLineEdit("General", "Auto change status");
	autoStatusText->setMaxLength(70);
	QString str=autoStatusText->text();
	//QVariant sizeStatusText=str.length();
	
	QLabel *autoStatusTextLength=ConfigDialog::getLabel("General", "0");
	autoStatusTextLength->setText(QString::number(GG_STATUS_DESCR_MAXSIZE - str.length()));
	//autoStatusTextLength->setText(sizeStatusText.asString());
	connect(autoStatusText, SIGNAL(textChanged(const QString&)), this, SLOT(setTextLength(const QString&)));
	kdebugf2();
}

void AutoAwaySlots::onApplyConfigDialog()
{
	kdebugf();
	
	QComboBox *autoChangeDescription=ConfigDialog::getComboBox("General", " ");
	int i=autoChangeDescription->currentItem();
	config_file.writeEntry("General", "AutoChangeDescription", i);
		
	if (config_file.readBoolEntry("General", "AutoChange"))
		on();
	else
		off();

	kdebugf2();
}

void AutoAwaySlots::changeAutoInvisibleTime(int i)
{	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	if (invisibleSpin->value()<i)
		invisibleSpin->setValue(i);
	
}
void AutoAwaySlots::changeAutoDisconnectTime(int i)
{	QSpinBox *disconnectSpin= ConfigDialog::getSpinBox("General", "Disconnect after ");
	if (disconnectSpin->value()<i)
		disconnectSpin->setValue(i);
	/* 	dodana reakcja na wpisanie wartosci mniejszej niz jest w away 
		mozna to zalatwic w osobnej funkcji zeby bylo ladniej ale niech na razie tak bedzie */
	QSpinBox *awaySpin= ConfigDialog::getSpinBox("General", "Set status to away after ");
	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	if ((invisibleSpin->value()<awaySpin->value()) && awaySpin->isEnabled())
		invisibleSpin->setValue(awaySpin->value());
}

void AutoAwaySlots::correctAutoDisconnectTime(int i)
{	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	QSpinBox *disconnectSpin= ConfigDialog::getSpinBox("General", "Disconnect after ");
	if ((disconnectSpin->value()<invisibleSpin->value()) && invisibleSpin->isEnabled())
		disconnectSpin->setValue(invisibleSpin->value());
}

void AutoAwaySlots::checkAutoInvisibleTime(bool b)
{	if (b)
	{	QSpinBox *awaySpin= ConfigDialog::getSpinBox("General", "Set status to away after ");
		QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
		if ((invisibleSpin->value()<awaySpin->value()) && awaySpin->isEnabled())
			invisibleSpin->setValue(awaySpin->value());
	}
}

void AutoAwaySlots::checkAutoDisconnectTime(bool b)
{	if (b)
	{	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
		QSpinBox *disconnectSpin= ConfigDialog::getSpinBox("General", "Disconnect after ");
		if ((disconnectSpin->value()<invisibleSpin->value()) && invisibleSpin->isEnabled())
			disconnectSpin->setValue(invisibleSpin->value());
	}
}

void AutoAwaySlots::setTextLength(const QString &str)
{	//QVariant sizeStatusText=str.length();
	QLabel *autoStatusTextLength=ConfigDialog::getLabel("General", "0");
	autoStatusTextLength->setText(QString::number(GG_STATUS_DESCR_MAXSIZE - str.length()));
//sizeStatusText.asString());
}

AutoAwaySlots::AutoAwaySlots() : QObject(NULL, "AutoAwaySlots")
{
}
