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
#include <qcursor.h>
#include <qfile.h>
#include <qhgroupbox.h>
#include <qspinbox.h>
#include <qtextstream.h>

#include <string.h>

#include "autoaway.h"
#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "kadu.h"
#include "message_box.h"
#include "misc.h"

AutoAwayTimer *autoaway_object=NULL;
AutoAwaySlots *autoawayslots=NULL;

extern "C" int autoaway_init()
{
	kdebugf();

	autoawayslots= new AutoAwaySlots(NULL, "autoawayslots");
	if (!gadu->status().isOffline())
		autoawayslots->on();

	ConfigDialog::registerSlotOnCreateTab("General", autoawayslots, SLOT(onCreateTabGeneral()));
	ConfigDialog::registerSlotOnApplyTab("General", autoawayslots, SLOT(onApplyTabGeneral()));

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

	ConfigDialog::unregisterSlotOnCreateTab("General", autoawayslots, SLOT(onCreateTabGeneral()));
	ConfigDialog::unregisterSlotOnApplyTab("General", autoawayslots, SLOT(onApplyTabGeneral()));

	delete autoawayslots;
	autoawayslots=NULL;

	kdebugf2();
}

AutoAwayTimer::AutoAwayTimer(QObject* parent, const char *name) : QTimer(parent, name), idleTime(0)
{
	didChangeStatus = false;
	didChangeDescription = false;
	action = (DescAction)config_file.readNumEntry("General", "AutoChangeDescription");
	actionText = config_file.readEntry("General", "AutoStatusText", "");
	checkInterval = config_file.readNumEntry("General","AutoAwayCheckTime");

	autoAwayTime = config_file.readNumEntry("General","AutoAwayTime");
	autoDisconnectTime = config_file.readNumEntry("General","AutoDisconnectTime");
	autoInvisibleTime = config_file.readNumEntry("General","AutoInvisibleTime");

	autoAwayEnabled = config_file.readBoolEntry("General","AutoAway");
	autoInvisibleEnabled = config_file.readBoolEntry("General","AutoInvisible");
	autoDisconnectEnabled = config_file.readBoolEntry("General","AutoDisconnect");

	restoreStatus = config_file.readBoolEntry("General","AutoRestoreStatus");

	qApp->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(checkIdleTime()));
	start(config_file.readNumEntry("General", "AutoAwayCheckTime")*1000, TRUE);
}

// jesli wciskamy klawisze lub poruszamy myszk± w obrêbie okna programu to zerujemy czas nieaktywno¶ci
bool AutoAwayTimer::eventFilter(QObject *o, QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove)
		idleTime = 0;
	return QObject::eventFilter(o, e);
}

//metoda wywo³ywana co sekundê(mo¿liwa zmiana w konfiguracji) w celu sprawdzenia czy mamy zmieniæ status
void AutoAwayTimer::checkIdleTime()
{
	const static int INTCOUNT=16;
	static unsigned long interrupts[INTCOUNT]={0};
	unsigned long currentInterrupts[INTCOUNT]={0};
	
	static QPoint MousePosition(0, 0);
	QPoint currentMousePosition;

	currentMousePosition = QCursor::pos();
	if (currentMousePosition != MousePosition)
		idleTime = 0;

	MousePosition = currentMousePosition;
	
//	sprawdzenie czy wzrosla liczba obsluzonych przerwan klawiatury lub myszki
	QFile f("/proc/interrupts");
	if (f.open(IO_ReadOnly))
	{
		QString line;
		QStringList strlist;

		QString intNum;
		int interrupt;
		
		QTextStream stream(&f);
		while (!stream.atEnd() && (line = stream.readLine()) != QString::null)
		{
			if (line.contains("i8042") || line.contains("keyboard") || line.contains("mouse", false))
			{
				strlist = QStringList::split(" ", line);
				
				intNum=strlist[0];
				intNum.truncate(intNum.length()-1);
				interrupt=intNum.toUInt();
				if (interrupt>=0 && interrupt<INTCOUNT)
					currentInterrupts[interrupt]=strlist[1].toULong();
			}
		}
		f.close();

		if (memcmp(interrupts, currentInterrupts, INTCOUNT*sizeof(interrupts[0]))!=0)
		{
			idleTime = 0;
			memcpy(interrupts, currentInterrupts, INTCOUNT*sizeof(interrupts[0]));
		}
	}

	idleTime += checkInterval;

	UserStatus currentStatus = gadu->currentStatus();

	//spradzamy czy mamy siê roz³±czyæ
	if (idleTime >= autoDisconnectTime && autoDisconnectEnabled)
	{
		kdebugmf(KDEBUG_INFO, "checking whether to disconnect, beforeAutoDisconnect = %d (%s)\n",
			currentStatus.index(), currentStatus.name().local8Bit().data());
		
		if (!didChangeStatus)
		{
			oldStatus.setStatus(currentStatus);	//zapamietuje status
			didChangeStatus = true;
		}
	
		if (didChangeDescription)	//jak potrzeba to zmienia status
			gadu->status().setOffline(gadu->status().description());
		else
		{	
			gadu->status().setOffline(changeDescription(currentStatus.description()));
			didChangeDescription = true;
		}
		kdebugmf(KDEBUG_INFO, "I am disconnected!\n");
	}
	//sprawdzamy czy mamy siê ukryæ
	else if (idleTime >= autoInvisibleTime && autoInvisibleEnabled)
	{	
		kdebugmf(KDEBUG_INFO, "checking whether to go invisible, beforeAutoInvisible = %d\n", gadu->status().index());
		if (currentStatus.isBusy() || currentStatus.isOnline())
		{
			if (!didChangeStatus)
			{	
				oldStatus.setStatus(currentStatus);
				didChangeStatus = true;
			}

			if (didChangeDescription)
				gadu->status().setInvisible(currentStatus.description());
			else
			{	
				gadu->status().setInvisible(changeDescription(currentStatus.description()));
				didChangeDescription = true;
			}
			kdebugmf(KDEBUG_INFO, "I am invisible!\n");
		}
	}
	//sprawdzamy czy mamy staæ sie "zajêci"
	else if (idleTime >= autoAwayTime && autoAwayEnabled)
	{
		kdebugmf(KDEBUG_INFO, "checking whether to go auto away, oldStatus = %d\n", oldStatus.index());
		
		if (currentStatus.isOnline())
		{
			if (!didChangeStatus)
			{
				oldStatus.setStatus(currentStatus);
				didChangeStatus = true;
			}
	
			if (didChangeDescription)
				gadu->status().setBusy(currentStatus.description());
			else
			{	
				gadu->status().setBusy(changeDescription(currentStatus.description()));
				didChangeDescription = true;
			}
			kdebugmf(KDEBUG_INFO, "I am away!\n");
		}
	}
	//je¶li zmieniali¶my status, a czas bezczynno¶ci jest odpowiednio ma³y, to przywracamy stary status
	else if (	((idleTime < autoAwayTime && currentStatus.isBusy()) ||
				(idleTime < autoInvisibleTime && currentStatus.isInvisible()) ||
				(idleTime < autoDisconnectTime && currentStatus.isOffline()))
				
	 			&& didChangeStatus)
	{
		kdebugmf(KDEBUG_INFO, "auto away cancelled\n");

		didChangeStatus = false;
		didChangeDescription = false;

		if (restoreStatus)
			gadu->status().setStatus(oldStatus);
	}

	start(checkInterval*1000, TRUE);
}

//dokonuje zmiany opisu zgodnie z wybrana opcja w konfiguracji
QString AutoAwayTimer::changeDescription(const QString &oldDescription)
{	
	QString newDescription;
	switch (action)	
	{	
		case NOTHING:
			newDescription = oldDescription;
			break;
		case REPLACE:
			newDescription = actionText;
			break;
		case PREPEND:
			newDescription = actionText+oldDescription;
			newDescription.truncate(70);
			break;
		case APPEND:
			newDescription = oldDescription;
			newDescription.truncate(70 - actionText.length() - 1);
			newDescription += actionText;
			break;
	}
	return newDescription;
}

void AutoAwaySlots::on()
{
	if (!autoaway_object && config_file.readBoolEntry("General", "AutoChange"))
		autoaway_object = new AutoAwayTimer(NULL, "autoaway_object");
}

void AutoAwaySlots::off()
{
	if (autoaway_object)
	{
		delete autoaway_object;
		autoaway_object = NULL;
	}
}

void AutoAwaySlots::onCreateTabGeneral()
{
	kdebugf();

	QHBox *awygrp = ConfigDialog::getHBox("General", "times");
	QHBox *awygrp2 = ConfigDialog::getHBox("General", "enables");
	QHGroupBox *awygrp3 = ConfigDialog::getHGroupBox("General", "AutoStatus Description");
	QCheckBox *b_autostatus = ConfigDialog::getCheckBox("General", "Enable AutoStatus");
	QCheckBox *b_autoaway = ConfigDialog::getCheckBox("General", "Enable autoaway");
	QCheckBox *b_autoinvisible = ConfigDialog::getCheckBox("General", "Enable autoinvisible");
	QCheckBox *b_autodisconnect = ConfigDialog::getCheckBox("General", "Enable autodisconnect");
	QCheckBox *b_autorestore = ConfigDialog::getCheckBox("General", "Autorestore status");
	/* wylaczenie AutoStatus wyszarza wszystko */
	awygrp->setEnabled(b_autostatus->isChecked());
	awygrp2->setEnabled(b_autostatus->isChecked());
	awygrp3->setEnabled(b_autostatus->isChecked());
	b_autorestore->setEnabled(b_autostatus->isChecked());
	QSpinBox *autoawayTime = ConfigDialog::getSpinBox("General", "Check idle every ");
	autoawayTime->setEnabled(b_autostatus->isChecked());
	/* wyszarzanie SpinBoxow przy wlaczonym AutoStatus */
	QSpinBox *autoawaySpin = ConfigDialog::getSpinBox("General", "Set status to away after ");
	autoawaySpin->setEnabled(b_autoaway->isChecked());
	QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	invisibleSpin->setEnabled(b_autoinvisible->isChecked());
	QSpinBox *disconnectSpin = ConfigDialog::getSpinBox("General", "Disconnect after ");
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
	
	QLineEdit *autoStatusText = ConfigDialog::getLineEdit("General", "Auto change status");
	autoStatusText->setMaxLength(70);
	QString str=autoStatusText->text();
	
	QLabel *autoStatusTextLength = ConfigDialog::getLabel("General", "0");
	autoStatusTextLength->setText(QString::number(GG_STATUS_DESCR_MAXSIZE - str.length()));
	connect(autoStatusText, SIGNAL(textChanged(const QString&)), this, SLOT(setTextLength(const QString&)));
	kdebugf2();
}

void AutoAwaySlots::onApplyTabGeneral()
{
	kdebugf();

	if (autoaway_object)
	{
		autoaway_object->didChangeStatus=false;
		autoaway_object->didChangeDescription=false;
		autoaway_object->action=(AutoAwayTimer::DescAction)ConfigDialog::getComboBox("General",  " ")->currentItem();
		autoaway_object->actionText=config_file.readEntry("General", "AutoStatusText", "");

		autoaway_object->checkInterval=config_file.readNumEntry("General","AutoAwayCheckTime");

		autoaway_object->autoAwayTime=config_file.readNumEntry("General","AutoAwayTime");
		autoaway_object->autoDisconnectTime=config_file.readNumEntry("General","AutoDisconnectTime");
		autoaway_object->autoInvisibleTime=config_file.readNumEntry("General","AutoInvisibleTime");

		autoaway_object->autoAwayEnabled=config_file.readBoolEntry("General","AutoAway");
		autoaway_object->autoInvisibleEnabled=config_file.readBoolEntry("General","AutoInvisible");
		autoaway_object->autoDisconnectEnabled=config_file.readBoolEntry("General","AutoDisconnect");

		autoaway_object->restoreStatus=config_file.readBoolEntry("General","AutoRestoreStatus");
	}

	if (config_file.readBoolEntry("General", "AutoChange"))
		on();
	else
		off();

	kdebugf2();
}

void AutoAwaySlots::changeAutoInvisibleTime(int i)
{
	QSpinBox *invisibleSpin= ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	if (invisibleSpin->value()<i)
		invisibleSpin->setValue(i);
}

void AutoAwaySlots::changeAutoDisconnectTime(int i)
{
	QSpinBox *disconnectSpin = ConfigDialog::getSpinBox("General", "Disconnect after ");
	if (disconnectSpin->value() < i)
		disconnectSpin->setValue(i);
	/* 	dodana reakcja na wpisanie wartosci mniejszej niz jest w away
		mozna to zalatwic w osobnej funkcji zeby bylo ladniej ale niech na razie tak bedzie */
	QSpinBox *awaySpin = ConfigDialog::getSpinBox("General", "Set status to away after ");
	QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	if ((invisibleSpin->value() < awaySpin->value()) && awaySpin->isEnabled())
		invisibleSpin->setValue(awaySpin->value());
}

void AutoAwaySlots::correctAutoDisconnectTime(int)
{
	QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	QSpinBox *disconnectSpin = ConfigDialog::getSpinBox("General", "Disconnect after ");
	if ((disconnectSpin->value() < invisibleSpin->value()) && invisibleSpin->isEnabled())
		disconnectSpin->setValue(invisibleSpin->value());
}

void AutoAwaySlots::checkAutoInvisibleTime(bool b)
{
	if (b)
	{
		QSpinBox *awaySpin = ConfigDialog::getSpinBox("General", "Set status to away after ");
		QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
		if ((invisibleSpin->value() < awaySpin->value()) && awaySpin->isEnabled())
			invisibleSpin->setValue(awaySpin->value());
	}
}

void AutoAwaySlots::checkAutoDisconnectTime(bool b)
{
	if (b)
	{
		QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
		QSpinBox *disconnectSpin = ConfigDialog::getSpinBox("General", "Disconnect after ");
		if ((disconnectSpin->value() < invisibleSpin->value()) && invisibleSpin->isEnabled())
			disconnectSpin->setValue(invisibleSpin->value());
	}
}

void AutoAwaySlots::setTextLength(const QString &str)
{
	QLabel *autoStatusTextLength = ConfigDialog::getLabel("General", "0");
	autoStatusTextLength->setText(QString::number(GG_STATUS_DESCR_MAXSIZE - str.length()));
}

AutoAwaySlots::AutoAwaySlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "General"), "GeneralTab");

	ConfigDialog::addHGroupBox("General", "General", "AutoStatus");
	ConfigDialog::addHBox("General", "AutoStatus", "autoStatus");
	ConfigDialog::addVBox("General", "autoStatus", "enables");
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autoaway"), "AutoAway", false);
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autoinvisible"), "AutoInvisible", false);
	ConfigDialog::addCheckBox("General", "enables",
			QT_TRANSLATE_NOOP("@default", "Enable autodisconnect"), "AutoDisconnect", false);
	ConfigDialog::addVBox("General", "autoStatus", "times", "", Advanced);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to away after "), "AutoAwayTime", 1, 10000, 1, 180);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to invisible after "), "AutoInvisibleTime", 1, 10000, 1, 600);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Disconnect after "), "AutoDisconnectTime", 1, 10000, 1, 1800);
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Enable AutoStatus"), "AutoChange", false);
	ConfigDialog::addSpinBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Check idle every "), "AutoAwayCheckTime", 1, 10000, 1, 5, "", "", Expert);
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Autorestore status"), "AutoRestoreStatus", true);
	ConfigDialog::addHGroupBox("General", "General", 
			QT_TRANSLATE_NOOP("@default", "AutoStatus Description"), "", Advanced);
	ConfigDialog::addHBox("General", "AutoStatus Description", "first");
	ConfigDialog::addLineEdit("General", "first",
			QT_TRANSLATE_NOOP("@default", "Auto change status"), "AutoStatusText", "");
	ConfigDialog::addHBox("General", "AutoStatus Description", "second");
	ConfigDialog::addLabel("General", "second", "0");

	ConfigDialog::addComboBox("General", "second", " ", "AutoChangeDescription",
		toStringList(	tr("Don't change the description"), 	//NOTHING
						tr("Change"),							//REPLACE
						tr("Add in front of description"),		//PREPEND
						tr("Add at the back of description")),	//APPEND
		toStringList("0","1","2","3"), "0");
	kdebugf2();
}

AutoAwaySlots::~AutoAwaySlots()
{
	kdebugf();
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
