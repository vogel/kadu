/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QSpinBox>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "misc/paths-provider.h"
#include "plugin/activation/plugin-activation-service.h"

#include "plugins/idle/idle-plugin.h"
#include "plugins/idle/idle.h"

#include "auto_hide.h"

AutoHide::AutoHide(QObject *parent) :
		ConfigurationUiHandler{parent},
		MyIdle{nullptr},
		IdleTime{0},
		Enabled{false}
{
}

AutoHide::~AutoHide()
{
}

bool AutoHide::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	connect(&Timer, SIGNAL(timeout()), this, SLOT(timerTimeoutSlot()));

	configurationUpdated();

	auto idleRootComponent = Core::instance()->pluginActivationService()->pluginRootComponent("idle");
	MyIdle = dynamic_cast<IdlePlugin *>(idleRootComponent)->idle();

	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/auto_hide.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	return true;
}

void AutoHide::done()
{
	Timer.stop();

	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/auto_hide.ui"));
}

void AutoHide::timerTimeoutSlot()
{
	if (Enabled)
	{
		if (MyIdle->secondsIdle() >= IdleTime)
		{
			KaduWindow *window = Core::instance()->kaduWindow();
			if (window->docked())
				window->window()->hide();
		}
	}
}

void AutoHide::configurationUpdated()
{
	IdleTime = Application::instance()->configuration()->deprecatedApi()->readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60);
	Enabled = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("PowerKadu", "auto_hide_use_auto_hide");

	if (Enabled && !Timer.isActive())
		Timer.start(1000);
	else if (!Enabled && Timer.isActive())
		Timer.stop();
}

void AutoHide::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("auto_hide/use_auto_hide"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time"), SLOT(setEnabled(bool)));

	static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time"))->setSpecialValueText(tr("Don't hide"));
}

Q_EXPORT_PLUGIN2(auto_hide, AutoHide)

#include "moc_auto_hide.cpp"
