/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "gui/widgets/configuration/configuration-widget.h"

#include "config_file.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"

#include "../idle/idle.h"

#include "autoaway.h"

/**
 * @ingroup autoaway
 * @{
 */
AutoAway *autoAway = 0;

extern "C" KADU_EXPORT int autoaway_init(bool firstLoad)
{
	kdebugf();

	autoAway = new AutoAway();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/autoaway.ui"), autoAway);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void autoaway_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/autoaway.ui"), autoAway);

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

void AutoAwayStatusChanger::changeStatus(Status &status)
{
	if (changeStatusTo == NoChangeStatus)
		return;

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
		status.setType(Status::Offline);
		status.setDescription(description);
		return;
	}

	if (status.isInvisible())
		return;

	if (changeStatusTo == ChangeStatusToInvisible)
	{
		status.setType(Status::Invisible);
		status.setDescription(description);
		return;
	}

	if (status.isBusy())
		return;

	if (changeStatusTo == ChangeStatusToBusy)
	{
		status.setType(Status::Busy);
		status.setDescription(description);
		return;
	}
}

void AutoAwayStatusChanger::setChangeStatusTo(ChangeStatusTo newChangeStatusTo)
{
	changeStatusTo = newChangeStatusTo;
	emit statusChanged();
}

void AutoAwayStatusChanger::setChangeDescriptionTo(ChangeDescriptionTo newChangeDescriptionTo, const QString &newDescriptionAddon)
{
	changeDescriptionTo = newChangeDescriptionTo;
	descriptionAddon = newDescriptionAddon;
}

AutoAway::AutoAway()
	: autoAwayStatusChanger(0), timer(0), updateDescripion(true)
{
//TODO 0.6.6:
	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();

	connect(gadu, SIGNAL(connected(Account *)), this, SLOT(on()));
	connect(gadu, SIGNAL(disconnected(Account *)), this, SLOT(off()));

	createDefaultConfiguration();
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
//TODO 0.6.6:
	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(connected(Account *)), this, SLOT(on()));
	disconnect(gadu, SIGNAL(disconnected(Account *)), this, SLOT(off()));

	qApp->removeEventFilter(this);
}

void AutoAway::on()
{
	if (!autoAwayStatusChanger)
	{
		autoAwayStatusChanger = new AutoAwayStatusChanger();
		status_changer_manager->registerStatusChanger(autoAwayStatusChanger);
	}

	autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));

	if (!timer)
	{
		timer = new QTimer();
		connect(timer, SIGNAL(timeout()), this, SLOT(checkIdleTime()));
		timer->start(config_file.readNumEntry("General", "AutoAwayCheckTime") * 1000, TRUE);
	}
}

void AutoAway::off()
{
	if (timer)
	{
		timer->stop();
		delete timer;
		timer = 0;
	}
}

//metoda wywo³ywana co sekundê(mo¿liwa zmiana w konfiguracji) w celu sprawdzenia czy mamy zmieniæ status
void AutoAway::checkIdleTime()
{
	kdebugf();

	idleTime = idle->secondsIdle();

	if (refreshStatusInterval > 0 && idleTime >= refreshStatusTime)
	{
		autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));
		refreshStatusTime = idleTime + refreshStatusInterval;
	}
 	else if (updateDescripion)
        {
		autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));
		updateDescripion = false;
	}

	if (idleTime >= autoDisconnectTime && autoDisconnectEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToOffline);
	else if (idleTime >= autoInvisibleTime && autoInvisibleEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToInvisible);
	else if (idleTime >= autoAwayTime && autoAwayEnabled)
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::ChangeStatusToBusy);
	else
	{
		autoAwayStatusChanger->setChangeStatusTo(AutoAwayStatusChanger::NoChangeStatus);
		updateDescripion = true;
	}

	if (idleTime < refreshStatusTime)
		refreshStatusTime = refreshStatusInterval;

	if (timer)
		timer->start(checkInterval * 1000, true);

	kdebugf2();
}

void AutoAway::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	autoAwaySpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoAway"));
	autoInvisibleSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoInvisible"));
	autoOfflineSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoOffline"));
	autoRefreshSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoRefresh"));

	descriptionTextLineEdit = dynamic_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionText"));

	parseStatusCheckBox = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/enableParseStatus"));

	connect(mainConfigurationWindow->widget()->widgetById("autoaway/enableAutoAway"), SIGNAL(toggled(bool)), autoAwaySpinBox, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("autoaway/enableAutoInvisible"), SIGNAL(toggled(bool)), autoInvisibleSpinBox, SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("autoaway/enableAutoOffline"), SIGNAL(toggled(bool)), autoOfflineSpinBox, SLOT(setEnabled(bool)));

	connect(autoAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoAwaySpinBoxValueChanged(int)));
	connect(autoInvisibleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoInvisibleSpinBoxValueChanged(int)));
	connect(autoOfflineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoOfflineSpinBoxValueChanged(int)));

	connect(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionChange"), SIGNAL(activated(int)), this, SLOT(descriptionChangeChanged(int)));

	autoRefreshSpinBox->setSpecialValueText(tr("Don't refresh"));
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
//TODO 0.6.6:
	Protocol *protocol = AccountManager::instance()->defaultAccount()->protocol();
	if ((autoAwayEnabled || autoInvisibleEnabled || autoDisconnectEnabled) && protocol->isConnected())
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
	if (parseAutoStatus)//TODO 0.6.6:
		return (KaduParser::parse(parseDescription, AccountManager::instance()->defaultAccount(), kadu->myself(), true));
	else
		return parseDescription;
}

void AutoAway::createDefaultConfiguration()
{
	config_file.addVariable("General", "AutoAway", true);
	config_file.addVariable("General", "AutoAwayCheckTime", 10);
	config_file.addVariable("General", "AutoAwayTime", 120);
	config_file.addVariable("General", "AutoChangeDescription", false);
	config_file.addVariable("General", "AutoDisconnect", false);
	config_file.addVariable("General", "AutoDisconnectTime", 3600);
	config_file.addVariable("General", "AutoInvisible", false);
	config_file.addVariable("General", "AutoInvisibleTime", 1800);
	config_file.addVariable("General", "AutoParseStatus", false);
	config_file.addVariable("General", "AutoRefreshStatusTime", 0);
	config_file.addVariable("General", "AutoStatusText", "");
}

/** @} */
