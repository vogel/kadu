/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2002 Tomasz Jarzynka (tomee@cpi.pl)
 * Copyright 2004, 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"
#include "parser/parser.h"
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
		StatusChanged{false},
		idleTime{},
		refreshStatusTime{},
		refreshStatusInterval{},
		autoAwaySpinBox{},
		autoExtendedAwaySpinBox{},
		autoInvisibleSpinBox{},
		autoOfflineSpinBox{},
		autoRefreshSpinBox{},
		descriptionTextLineEdit{}
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

	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	return true;
}

void AutoAway::done()
{
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/autoaway.ui"));

	StatusChangerManager::instance()->unregisterStatusChanger(autoAwayStatusChanger);
}

AutoAwayStatusChanger::ChangeStatusTo AutoAway::changeStatusTo()
{
	idleTime = IdlePlugin::idle()->secondsIdle();

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

	idleTime = IdlePlugin::idle()->secondsIdle();

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
	checkInterval = config_file.readUnsignedNumEntry("General","AutoAwayCheckTime");
	refreshStatusTime = config_file.readUnsignedNumEntry("General","AutoRefreshStatusTime");
	autoAwayTime = config_file.readUnsignedNumEntry("General","AutoAwayTimeMinutes")*60;
	autoExtendedAwayTime = config_file.readUnsignedNumEntry("General","AutoExtendedAwayTimeMinutes")*60;
	autoDisconnectTime = config_file.readUnsignedNumEntry("General","AutoDisconnectTimeMinutes")*60;
	autoInvisibleTime = config_file.readUnsignedNumEntry("General","AutoInvisibleTimeMinutes")*60;

	autoAwayEnabled = config_file.readBoolEntry("General","AutoAway");
	autoExtendedAwayEnabled = config_file.readBoolEntry("General","AutoExtendedAway");
	autoInvisibleEnabled = config_file.readBoolEntry("General","AutoInvisible");
	autoDisconnectEnabled = config_file.readBoolEntry("General","AutoDisconnect");
	parseAutoStatus = config_file.readBoolEntry("General", "ParseStatus");

	refreshStatusInterval = refreshStatusTime;

	autoStatusText = config_file.readEntry("General", "AutoStatusText");
	DescriptionAddon = parseDescription(autoStatusText);

	changeTo = (AutoAwayStatusChanger::ChangeDescriptionTo)config_file.readNumEntry("General", "AutoChangeDescription");

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
	int ret = config_file.readUnsignedNumEntry("General", name, def * 60);
	// This AutoAwayTimesDenominated thing was living shortly in 1.0-git.
	return config_file.readBoolEntry("General", "AutoAwayTimesDenominated", false)
			? ret
			: (ret + 59) / 60;
}

void AutoAway::createDefaultConfiguration()
{
	config_file.addVariable("General", "AutoAway", true);
	config_file.addVariable("General", "AutoAwayCheckTime", 10);
	config_file.addVariable("General", "AutoAwayTimeMinutes", denominatedInverval("AutoAwayTime", 5));
	config_file.addVariable("General", "AutoExtendedAway", true);
	config_file.addVariable("General", "AutoExtendedAwayTimeMinutes", denominatedInverval("AutoExtendedAwayTime", 15));
	config_file.addVariable("General", "AutoChangeDescription", 0);
	config_file.addVariable("General", "AutoDisconnect", false);
	config_file.addVariable("General", "AutoDisconnectTimeMinutes", denominatedInverval("AutoDisconnectTime", 60));
	config_file.addVariable("General", "AutoInvisible", false);
	config_file.addVariable("General", "AutoInvisibleTimeMinutes", denominatedInverval("AutoInvisibleTime", 30));
	config_file.addVariable("General", "AutoRefreshStatusTime", 0);
	config_file.addVariable("General", "AutoStatusText", QString());

	// AutoAwayCheckTime has been mistakenly denominated in 1.0-git.
	if (0 == config_file.readUnsignedNumEntry("General", "AutoAwayCheckTime"))
		config_file.writeEntry("General", "AutoAwayCheckTime", 10);
}


Q_EXPORT_PLUGIN2(autoaway, AutoAway)

/** @} */

#include "moc_autoaway.cpp"
