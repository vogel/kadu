/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"
#include "status/status-changer-manager.h"
#include "debug.h"

#include "modules/idle/idle.h"

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

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/autoaway.ui"));
	MainConfigurationWindow::registerUiHandler(autoAway);

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void autoaway_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/autoaway.ui"));
	MainConfigurationWindow::unregisterUiHandler(autoAway);

	delete autoAway;
	autoAway = 0;

	kdebugf2();
}

AutoAway::AutoAway() :
		autoAwayStatusChanger(0), timer(0), updateDescripion(true)
{
	autoAwayStatusChanger = new AutoAwayStatusChanger();
	StatusChangerManager::instance()->registerStatusChanger(autoAwayStatusChanger);

	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(checkIdleTime()));

	timer->setInterval(config_file.readNumEntry("General", "AutoAwayCheckTime", 5) * 1000);
	timer->setSingleShot(true);
	timer->start();

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
		StatusChangerManager::instance()->unregisterStatusChanger(autoAwayStatusChanger);
		delete autoAwayStatusChanger;
		autoAwayStatusChanger = 0;
	}

	qApp->removeEventFilter(this);
}

//metoda wywo�ywana co sekund�(mo�liwa zmiana w konfiguracji) w celu sprawdzenia czy mamy zmieni� status
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
	{
		timer->setInterval(checkInterval * 1000);
		timer->setSingleShot(true);
		timer->start();
	}

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

	autoAwayStatusChanger->setChangeDescriptionTo(changeTo, parseDescription(autoStatusText));
	timer->setInterval(config_file.readNumEntry("General", "AutoAwayCheckTime") * 1000);
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
// 	if (parseAutoStatus)//TODO 0.6.6:
// 		return (KaduParser::parse(parseDescription, AccountManager::instance()->defaultAccount(), kadu->myself(), true));
// 	else
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
