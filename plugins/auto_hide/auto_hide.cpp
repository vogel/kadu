/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QSpinBox>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/kadu-window.h"
#include "misc/path-conversion.h"

#include "plugins/idle/idle-plugin.h"
#include "plugins/idle/idle.h"

#include "auto_hide.h"

AutoHide::AutoHide(QObject *parent) :
		ConfigurationUiHandler(parent), IdleTime(0)
{
	connect(&Timer, SIGNAL(timeout()), this, SLOT(timerTimeoutSlot()));

	configurationUpdated();
}

AutoHide::~AutoHide()
{
	Timer.stop();
}

int AutoHide::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/auto_hide.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	return 0;
}

void AutoHide::done()
{
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/auto_hide.ui"));
}

void AutoHide::timerTimeoutSlot()
{
	if (Enabled)
	{
		if (IdlePlugin::idle()->secondsIdle() >= IdleTime)
		{
			KaduWindow *window = Core::instance()->kaduWindow();
			if (window->docked())
				window->window()->hide();
		}
	}
}

void AutoHide::configurationUpdated()
{
	IdleTime = config_file.readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60);
	Enabled = config_file.readBoolEntry("PowerKadu", "auto_hide_use_auto_hide");

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
