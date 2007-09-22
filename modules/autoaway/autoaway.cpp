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
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "message_box.h"
#include "misc.h"

/**
 * @ingroup autoaway
 * @{
 */
AutoAway *autoAway = 0;

extern "C" int autoaway_init()
{
	kdebugf();

	autoAway = new AutoAway();
	if (!gadu->status().isOffline())
		autoAway->on();

	QObject::connect(gadu, SIGNAL(disconnected()), autoAway, SLOT(off()));
	QObject::connect(gadu, SIGNAL(connected()), autoAway, SLOT(on()));

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/autoaway.ui"), autoAway);

	kdebugf2();
	return 0;
}

extern "C" void autoaway_close()
{
	kdebugf();

	autoAway->off();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/autoaway.ui"), autoAway);

	QObject::disconnect(gadu, SIGNAL(disconnected()), autoAway, SLOT(off()));
	QObject::disconnect(gadu, SIGNAL(connected()), autoAway, SLOT(on()));

	delete autoAway;
	autoAway = 0;

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
		// TODO: make it pretty
		kadu->setOffline(description);
// 		status.setOffline(description);
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

AutoAway::AutoAway(QObject* parent, const char *name)
	: autoAwayStatusChanger(0), timer(0)
{
	configurationUpdated();
}

AutoAway::~AutoAway()
{
	if (timer)
	{
		delete timer;
		timer = 0;
	}

	if (autoAwayStatusChanger)
	{
		status_changer_manager->unregisterStatusChanger(autoAwayStatusChanger);
		delete autoAwayStatusChanger;
		autoAwayStatusChanger = 0;
	}
}

void AutoAway::on()
{
	if (!autoAwayStatusChanger)
		autoAwayStatusChanger = new AutoAwayStatusChanger();

	autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));

	status_changer_manager->registerStatusChanger(autoAwayStatusChanger);

	qApp->installEventFilter(this);

	if (!timer)
		timer = new QTimer();

	connect(timer, SIGNAL(timeout()), this, SLOT(checkIdleTime()));
	timer->start(config_file.readNumEntry("General", "AutoAwayCheckTime") * 1000, TRUE);
}

void AutoAway::off()
{
	if (timer)
	{
		timer->stop();
		delete timer;
		timer = 0;
	}

	qApp->removeEventFilter(this);
}

// jesli wciskamy klawisze lub poruszamy myszk± w obrêbie okna programu to zerujemy czas nieaktywno¶ci
bool AutoAway::eventFilter(QObject *o, QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove)
		idleTime = 0;
	return QObject::eventFilter(o, e);
}

//metoda wywo³ywana co sekundê(mo¿liwa zmiana w konfiguracji) w celu sprawdzenia czy mamy zmieniæ status
void AutoAway::checkIdleTime()
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

	if (refreshStatusInterval > 0 && idleTime >= refreshStatusTime)
	{
		autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));
		refreshStatusTime = idleTime + refreshStatusInterval;
	}

	if (idleTime >= autoDisconnectTime && autoDisconnectEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToOffline);
	else if (idleTime >= autoInvisibleTime && autoInvisibleEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToInvisible);
	else if (idleTime >= autoAwayTime && autoAwayEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToBusy);
	else
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::NoChangeStatus);

	if (idleTime < refreshStatusTime)
		refreshStatusTime = refreshStatusInterval;

	if (timer)
		timer->start(checkInterval * 1000, TRUE);

	kdebugf2();
}

void AutoAway::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	autoAwaySpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("autoaway/autoAway"));
	autoInvisibleSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("autoaway/autoInvisible"));
	autoOfflineSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("autoaway/autoOffline"));
	autoRefreshSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("autoaway/autoRefresh"));

	descriptionTextLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widgetById("autoaway/descriptionText"));

	parseStatusCheckBox = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("autoaway/enableParseStatus"));

	connect(mainConfigurationWindow->widgetById("autoaway/enableAutoAway"), SIGNAL(toggled(bool)), autoAwaySpinBox, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("autoaway/enableAutoInvisible"), SIGNAL(toggled(bool)), autoInvisibleSpinBox, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("autoaway/enableAutoOffline"), SIGNAL(toggled(bool)), autoOfflineSpinBox, SLOT(setEnabled(bool)));

	connect(autoAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoAwaySpinBoxValueChanged(int)));
	connect(autoInvisibleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoInvisibleSpinBoxValueChanged(int)));
	connect(autoOfflineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoOfflineSpinBoxValueChanged(int)));

	connect(mainConfigurationWindow->widgetById("autoaway/descriptionChange"), SIGNAL(activated(int)), this, SLOT(descriptionChangeChanged(int)));
}

void AutoAway::configurationUpdated()
{
	checkInterval = config_file.readUnsignedNumEntry("General","AutoAwayCheckTime");

	autoAwayTime = config_file.readUnsignedNumEntry("General","AutoAwayTime");
	autoDisconnectTime = config_file.readUnsignedNumEntry("General","AutoDisconnectTime");
	autoInvisibleTime = config_file.readUnsignedNumEntry("General","AutoInvisibleTime");

	autoAwayEnabled = config_file.readBoolEntry("General","AutoAway");
	autoInvisibleEnabled = config_file.readBoolEntry("General","AutoInvisible");
	autoDisconnectEnabled = config_file.readBoolEntry("General","AutoDisconnect");
	parseAutoStatus = config_file.readBoolEntry("General", "AutoParseStatus");

	refreshStatusTime = config_file.readUnsignedNumEntry("General","AutoRefreshStatusTime");
	refreshStatusInterval = config_file.readUnsignedNumEntry("General","AutoRefreshStatusTime");

	autoStatusText = config_file.readEntry("General", "AutoStatusText");

	changeTo = (AutoAwayStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("General", "AutoChangeDescription");

	if (autoAwayEnabled || autoInvisibleEnabled || autoDisconnectEnabled)
		on();
	else
		off();
}

void AutoAway::autoAwaySpinBoxValueChanged(int value)
{
	if (autoInvisibleSpinBox->value() < value)
		autoInvisibleSpinBox->setValue(value);
}

void AutoAway::autoInvisibleSpinBoxValueChanged(int value)
{
	if (autoAwaySpinBox->value() > value)
		autoAwaySpinBox->setValue(value);
	if (autoOfflineSpinBox->value() < value)
		autoOfflineSpinBox->setValue(value);
}

void AutoAway::autoOfflineSpinBoxValueChanged(int value)
{
	if (autoInvisibleSpinBox->value() > value)
		autoInvisibleSpinBox->setValue(value);
}

void AutoAway::descriptionChangeChanged(int index)
{
	descriptionTextLineEdit->setEnabled(index != 0);
	autoRefreshSpinBox->setEnabled(index != 0);
	parseStatusCheckBox->setEnabled(index != 0);
}

QString AutoAway::parseDescription(const QString &parseDescription)
{
	if (parseAutoStatus)
    		return (KaduParser::parse(parseDescription, kadu->myself(), true));
	else
		return parseDescription;
}

/** @} */
