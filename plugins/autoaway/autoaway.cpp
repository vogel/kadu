/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

#include "accounts/account.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "parser/parser.h"
#include "plugin/activation/plugin-activation-service.h"
#include "status/status-changer-manager.h"
#include "debug.h"

#include "plugins/idle/idle-plugin.h"
#include "plugins/idle/idle.h"

#include "autoaway.h"

/**
 * @ingroup autoaway
 * @{
 */

AutoAway::AutoAway() :
		autoAwayStatusChanger{},
		timer{},
		checkInterval{},
		autoAwayTime{},
		autoExtendedAwayTime{},
		autoDisconnectTime{},
		autoInvisibleTime{},
		autoAwayEnabled{},
		autoExtendedAwayEnabled{},
		autoInvisibleEnabled{},
		autoDisconnectEnabled{},
		parseAutoStatus{},
		StatusChanged{false},
		idle{},
		idleTime{},
		refreshStatusTime{},
		refreshStatusInterval{},
		autoAwaySpinBox{},
		autoExtendedAwaySpinBox{},
		autoInvisibleSpinBox{},
		autoOfflineSpinBox{},
		autoRefreshSpinBox{},
		descriptionTextLineEdit{},
		changeTo{AutoAwayStatusChanger::NoChangeDescription}
{
}

AutoAway::~AutoAway()
{
}

bool AutoAway::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	autoAwayStatusChanger = new AutoAwayStatusChanger(this, this);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkIdleTime()));

	createDefaultConfiguration();
	configurationUpdated();

	StatusChangerManager::instance()->registerStatusChanger(autoAwayStatusChanger);

	auto idleRootComponent = Core::instance()->pluginActivationService()->pluginRootComponent("idle");
	idle = dynamic_cast<IdlePlugin *>(idleRootComponent)->idle();

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	return true;
}

void AutoAway::done()
{
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));

	StatusChangerManager::instance()->unregisterStatusChanger(autoAwayStatusChanger);
}

AutoAwayStatusChanger::ChangeStatusTo AutoAway::changeStatusTo()
{
	idleTime = idle->secondsIdle();

	if (idleTime >= autoDisconnectTime && autoDisconnectEnabled)
		return AutoAwayStatusChanger::ChangeStatusToOffline;
	else if (idleTime >= autoInvisibleTime && autoInvisibleEnabled)
		return AutoAwayStatusChanger::ChangeStatusToInvisible;
	else if (idleTime >= autoExtendedAwayTime && autoExtendedAwayEnabled)
		return AutoAwayStatusChanger::ChangeStatusToExtendedAway;
	else if (idleTime >= autoAwayTime && autoAwayEnabled)
		return AutoAwayStatusChanger::ChangeStatusToAway;
	else
		return AutoAwayStatusChanger::NoChangeStatus;
}

AutoAwayStatusChanger::ChangeDescriptionTo AutoAway::changeDescriptionTo()
{
	AutoAwayStatusChanger::ChangeStatusTo currentChangeStatusTo = changeStatusTo();
	if (AutoAwayStatusChanger::NoChangeStatus == currentChangeStatusTo)
		return AutoAwayStatusChanger::NoChangeDescription;

	return changeTo;
}

QString AutoAway::descriptionAddon() const
{
	return DescriptionAddon;
}

void AutoAway::checkIdleTime()
{
	kdebugf();

	idleTime = idle->secondsIdle();

	if (refreshStatusInterval > 0 && idleTime >= refreshStatusTime)
	{
		DescriptionAddon = parseDescription(autoStatusText);
		refreshStatusTime = idleTime + refreshStatusInterval;
	}

	if (changeStatusTo() != AutoAwayStatusChanger::NoChangeStatus)
	{
		autoAwayStatusChanger->update();
		StatusChanged = true;
	}
	else if (StatusChanged)
	{
		StatusChanged = false;
		autoAwayStatusChanger->update();
	}

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
	autoAwaySpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoAway"));
	autoExtendedAwaySpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoExtendedAway"));
	autoInvisibleSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoInvisible"));
	autoOfflineSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoOffline"));
	autoRefreshSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoRefresh"));

	descriptionTextLineEdit = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionText"));

	connect(autoAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoAwaySpinBoxValueChanged(int)));
	connect(autoExtendedAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoExtendedAwaySpinBoxValueChanged(int)));
	connect(autoInvisibleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoInvisibleSpinBoxValueChanged(int)));
	connect(autoOfflineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoOfflineSpinBoxValueChanged(int)));

	connect(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionChange"), SIGNAL(activated(int)), this, SLOT(descriptionChangeChanged(int)));

	autoRefreshSpinBox->setSpecialValueText(tr("Don't refresh"));
}

void AutoAway::configurationUpdated()
{
	checkInterval = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoAwayCheckTime");
	refreshStatusTime = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoRefreshStatusTime");
	autoAwayTime = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoAwayTimeMinutes")*60;
	autoExtendedAwayTime = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoExtendedAwayTimeMinutes")*60;
	autoDisconnectTime = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoDisconnectTimeMinutes")*60;
	autoInvisibleTime = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General","AutoInvisibleTimeMinutes")*60;

	autoAwayEnabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General","AutoAway");
	autoExtendedAwayEnabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General","AutoExtendedAway");
	autoInvisibleEnabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General","AutoInvisible");
	autoDisconnectEnabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General","AutoDisconnect");
	parseAutoStatus = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ParseStatus");

	refreshStatusInterval = refreshStatusTime;

	autoStatusText = Application::instance()->configuration()->deprecatedApi()->readEntry("General", "AutoStatusText");
	DescriptionAddon = parseDescription(autoStatusText);

	changeTo = (AutoAwayStatusChanger::ChangeDescriptionTo)Application::instance()->configuration()->deprecatedApi()->readNumEntry("General", "AutoChangeDescription");

	autoAwayStatusChanger->update();

	if (autoAwayEnabled || autoExtendedAwayEnabled || autoInvisibleEnabled || autoDisconnectEnabled)
	{
		timer->setInterval(checkInterval * 1000);
		timer->setSingleShot(true);
		timer->start();
	}
	else
		timer->stop();
}

void AutoAway::autoAwaySpinBoxValueChanged(int value)
{
	if (autoInvisibleSpinBox->value() < value)
		autoInvisibleSpinBox->setValue(value);
	if (autoExtendedAwaySpinBox->value() < value)
		autoExtendedAwaySpinBox->setValue(value);
}

void AutoAway::autoExtendedAwaySpinBoxValueChanged(int value)
{
	if (autoInvisibleSpinBox->value() < value)
		autoInvisibleSpinBox->setValue(value);
	if (autoAwaySpinBox->value() > value)
		autoAwaySpinBox->setValue(value);
}

void AutoAway::autoInvisibleSpinBoxValueChanged(int value)
{
	if (autoAwaySpinBox->value() > value)
		autoAwaySpinBox->setValue(value);
	if (autoExtendedAwaySpinBox->value() > value)
		autoExtendedAwaySpinBox->setValue(value);
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
}

QString AutoAway::parseDescription(const QString &parseDescription)
{
	if (parseAutoStatus)
		return (Parser::parse(parseDescription, Talkable(Core::instance()->myself()), ParserEscape::HtmlEscape));
	else
		return parseDescription;
}

static int denominatedInverval(const QString &name, unsigned int def)
{
	int ret = Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General", name, def * 60);
	// This AutoAwayTimesDenominated thing was living shortly in 1.0-git.
	return Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "AutoAwayTimesDenominated", false)
			? ret
			: (ret + 59) / 60;
}

void AutoAway::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoAway", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoAwayCheckTime", 10);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoAwayTimeMinutes", denominatedInverval("AutoAwayTime", 5));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoExtendedAway", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoExtendedAwayTimeMinutes", denominatedInverval("AutoExtendedAwayTime", 15));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoChangeDescription", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoDisconnect", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoDisconnectTimeMinutes", denominatedInverval("AutoDisconnectTime", 60));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoInvisible", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoInvisibleTimeMinutes", denominatedInverval("AutoInvisibleTime", 30));
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoRefreshStatusTime", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "AutoStatusText", QString());

	// AutoAwayCheckTime has been mistakenly denominated in 1.0-git.
	if (0 == Application::instance()->configuration()->deprecatedApi()->readUnsignedNumEntry("General", "AutoAwayCheckTime"))
		Application::instance()->configuration()->deprecatedApi()->writeEntry("General", "AutoAwayCheckTime", 10);
}


Q_EXPORT_PLUGIN2(autoaway, AutoAway)

/** @} */

#include "moc_autoaway.cpp"
