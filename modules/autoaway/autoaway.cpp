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

/**
 * @ingroup autoaway
 * @{
 */
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

	QObject::connect(gadu, SIGNAL(disconnected()), autoawayslots, SLOT(off()));
	QObject::connect(gadu, SIGNAL(connected()), autoawayslots, SLOT(on()));

	kdebugf2();
	return 0;
}

extern "C" void autoaway_close()
{
	kdebugf();
	autoawayslots->off();

	QObject::disconnect(gadu, SIGNAL(disconnected()), autoawayslots, SLOT(off()));
	QObject::disconnect(gadu, SIGNAL(connected()), autoawayslots, SLOT(on()));

	ConfigDialog::unregisterSlotOnCreateTab("General", autoawayslots, SLOT(onCreateTabGeneral()));
	ConfigDialog::unregisterSlotOnApplyTab("General", autoawayslots, SLOT(onApplyTabGeneral()));

	delete autoawayslots;
	autoawayslots=NULL;

	kdebugf2();
}

AutoAwayStatusChanger::AutoAwayStatusChanger()
	: StatusChanger(900), changeStatusTo(NoChangeStatus), changeDescriptionTo(NoChangeDescription)
{
}

AutoAwayStatusChanger::~AutoAwayStatusChanger()
{
}

void AutoAwayStatusChanger::changeStatus(UserStatus &status)
{
	if (status.isOffline())
		return;

	QString description = status.description();
	switch (changeDescriptionTo)
	{
		case NoChangeDescription:
			break;

		case ChangeDescriptionPrepend:
			description = descriptionAddon + description;
			break;

		case ChangeDescriptionReplace:
			description = descriptionAddon;
			break;

		case ChangeDescriptionAppend:
			description = description + descriptionAddon;
			break;
	}

	if (changeStatusTo == ChangeStatusToOffline)
	{
		status.setOffline(description);
		return;
	}

	if (status.isInvisible())
		return;

	if (changeStatusTo == ChangeStatusToInvisible)
	{
		status.setInvisible(description);
		return;
	}

	if (status.isBusy())
		return;

	if (changeStatusTo == ChangeStatusToBusy)
	{
		status.setBusy(description);
		return;
	}
}

void AutoAwayStatusChanger::setChangeStatusTo(ChangeStatusTo newChangeStatusTo)
{
	if (changeStatusTo != newChangeStatusTo)
	{
		changeStatusTo = newChangeStatusTo;
		emit statusChanged();
	}
}

void AutoAwayStatusChanger::setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon)
{
	if (changeDescriptionTo != newChangeDescriptionTo || descriptionAddon != newDescriptionAddon)
	{
		changeDescriptionTo = newChangeDescriptionTo;
		descriptionAddon = newDescriptionAddon;
		emit statusChanged();
	}
}

AutoAwayTimer::AutoAwayTimer(AutoAwayStatusChanger *autoAwayStatusChanger, QObject* parent, const char *name) : QTimer(parent, name),
	autoAwayStatusChanger(autoAwayStatusChanger),
	checkInterval(config_file.readUnsignedNumEntry("General", "AutoAwayCheckTime")),
	autoAwayTime(config_file.readUnsignedNumEntry("General", "AutoAwayTime")),
	autoDisconnectTime(config_file.readUnsignedNumEntry("General", "AutoDisconnectTime")),
	autoInvisibleTime(config_file.readUnsignedNumEntry("General", "AutoInvisibleTime")),
	autoAwayEnabled(config_file.readBoolEntry("General", "AutoAway")),
	autoInvisibleEnabled(config_file.readBoolEntry("General", "AutoInvisible")),
	autoDisconnectEnabled(config_file.readBoolEntry("General", "AutoDisconnect")),
	oldStatus(), idleTime(0)
{
	qApp->installEventFilter(this);
	connect(this, SIGNAL(timeout()), SLOT(checkIdleTime()));
	start(config_file.readNumEntry("General", "AutoAwayCheckTime") * 1000, TRUE);
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
	kdebugf();

	const static int INTCOUNT = 16;
	static unsigned long interrupts[INTCOUNT] = {0};
	unsigned long currentInterrupts[INTCOUNT] = {0};

	static QPoint MousePosition(0, 0);
	QPoint currentMousePosition;

	currentMousePosition = QCursor::pos();
	if (currentMousePosition != MousePosition)
	{
		MousePosition = currentMousePosition;
		idleTime = 0;
	}

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

				intNum = strlist[0];
				intNum.truncate(intNum.length()-1);
				interrupt = intNum.toUInt();
				if (interrupt>=0 && interrupt<INTCOUNT)
					currentInterrupts[interrupt] = strlist[1].toULong();
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

	if (idleTime >= autoDisconnectTime && autoDisconnectEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToOffline);
	else if (idleTime >= autoInvisibleTime && autoInvisibleEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToInvisible);
	else if (idleTime >= autoAwayTime && autoAwayEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToBusy);
	else
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::NoChangeStatus);

	start(checkInterval * 1000, TRUE);

	kdebugf2();
}

void AutoAwaySlots::on()
{
	if (!autoaway_object && config_file.readBoolEntry("General", "AutoChange"))
		autoaway_object = new AutoAwayTimer(autoAwayStatusChanger, NULL, "autoaway_object");
}

void AutoAwaySlots::off()
{
	if (autoaway_object /*&& !autoaway_object->didChangeStatus*/)
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
	/* wylaczenie AutoStatus wyszarza wszystko */
	awygrp->setEnabled(b_autostatus->isChecked());
	awygrp2->setEnabled(b_autostatus->isChecked());
	awygrp3->setEnabled(b_autostatus->isChecked());
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
	autoawaySpin->setSuffix(" s");
	invisibleSpin->setSuffix(" s");
	disconnectSpin->setSuffix(" s");
	autoawayTime->setSuffix(" s");

	QLineEdit *autoStatusText = ConfigDialog::getLineEdit("General", "Auto change status");
	autoStatusText->setMaxLength(70);
	QString str = autoStatusText->text();

	QLabel *autoStatusTextLength = ConfigDialog::getLabel("General", "0");
	autoStatusTextLength->setText(QString::number(GG_STATUS_DESCR_MAXSIZE - str.length()));
	connect(autoStatusText, SIGNAL(textChanged(const QString&)), this, SLOT(setTextLength(const QString&)));
	kdebugf2();
}

void AutoAwaySlots::onApplyTabGeneral()
{
	kdebugf();

	autoAwayStatusChanger->setChangeDescriptionTo(
		(AutoAwayStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("General", "AutoChangeDescription"),
		config_file.readEntry("General", "AutoStatusText")
	);

	if (autoaway_object)
	{
		autoaway_object->checkInterval = config_file.readNumEntry("General","AutoAwayCheckTime");

		autoaway_object->autoAwayTime = config_file.readNumEntry("General","AutoAwayTime");
		autoaway_object->autoDisconnectTime = config_file.readNumEntry("General","AutoDisconnectTime");
		autoaway_object->autoInvisibleTime = config_file.readNumEntry("General","AutoInvisibleTime");

		autoaway_object->autoAwayEnabled = config_file.readBoolEntry("General","AutoAway");
		autoaway_object->autoInvisibleEnabled = config_file.readBoolEntry("General","AutoInvisible");
		autoaway_object->autoDisconnectEnabled = config_file.readBoolEntry("General","AutoDisconnect");
	}

	if (config_file.readBoolEntry("General", "AutoChange"))
		on();
	else
		off();

	kdebugf2();
}

void AutoAwaySlots::changeAutoInvisibleTime(int i)
{
	QSpinBox *invisibleSpin = ConfigDialog::getSpinBox("General", "Set status to invisible after ");
	if (invisibleSpin->value() < i)
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
	ConfigDialog::addVBox("General", "autoStatus", "times", 0, Advanced);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to away after "), "AutoAwayTime", 1, 10000, 1, 180);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Set status to invisible after "), "AutoInvisibleTime", 1, 10000, 1, 600);
	ConfigDialog::addSpinBox("General", "times",
			QT_TRANSLATE_NOOP("@default", "Disconnect after "), "AutoDisconnectTime", 1, 10000, 1, 1800);
	ConfigDialog::addCheckBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Enable AutoStatus"), "AutoChange", false);
	ConfigDialog::addSpinBox("General", "Status",
			QT_TRANSLATE_NOOP("@default", "Check idle every "), "AutoAwayCheckTime", 1, 10000, 1, 5, 0, 0, Expert);
	ConfigDialog::addHGroupBox("General", "General",
			QT_TRANSLATE_NOOP("@default", "AutoStatus Description"), 0, Advanced);
	ConfigDialog::addHBox("General", "AutoStatus Description", "first");
	ConfigDialog::addLineEdit("General", "first",
			QT_TRANSLATE_NOOP("@default", "Auto change status"), "AutoStatusText");
	ConfigDialog::addHBox("General", "AutoStatus Description", "second");
	ConfigDialog::addLabel("General", "second", "0");

	ConfigDialog::addComboBox("General", "second", " ", "AutoChangeDescription",
		toStringList(	tr("Don't change the description"), 	//NOTHING
						tr("Change"),							//REPLACE
						tr("Add in front of description"),		//PREPEND
						tr("Add at the back of description")),	//APPEND
		toStringList("0","1","2","3"), "0");

	autoAwayStatusChanger = new AutoAwayStatusChanger();
	autoAwayStatusChanger->setChangeDescriptionTo(
		(AutoAwayStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("General", "AutoChangeDescription"),
		config_file.readEntry("General", "AutoStatusText")
	);
	status_changer_manager->registerStatusChanger(autoAwayStatusChanger);

	kdebugf2();
}

AutoAwaySlots::~AutoAwaySlots()
{
	kdebugf();

	status_changer_manager->unregisterStatusChanger(autoAwayStatusChanger);
	delete autoAwayStatusChanger;
	autoAwayStatusChanger = NULL;

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
	ConfigDialog::removeControl("General", "Auto change status");
	ConfigDialog::removeControl("General", " ");
	ConfigDialog::removeControl("General", "0");
	ConfigDialog::removeControl("General", "first");
	ConfigDialog::removeControl("General", "second");
	ConfigDialog::removeControl("General", "AutoStatus Description");
	kdebugf2();
}

/** @} */

