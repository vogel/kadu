/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include <QtGui/QCursor>
#include <QtCore/QFile>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtCore/QTextStream>

#include "auto_hide.h"

#include "core/core.h"
#include "gui/windows/kadu-window.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "debug.h"
#include "../idle/idle.h"
#include "misc/path-conversion.h"

AutoHide *autoHide;

extern "C" KADU_EXPORT int auto_hide_init()
{
	kdebugf();

	autoHide = new AutoHide();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/auto_hide.ui"));
	MainConfigurationWindow::registerUiHandler(autoHide);

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void auto_hide_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/auto_hide.ui"));
	MainConfigurationWindow::unregisterUiHandler(autoHide);

	delete autoHide;
	autoHide = 0;

	kdebugf2();
}


AutoHide::AutoHide(QObject *parent)
: QObject(parent), idleTime(0)
{
	kdebugf();
	
	connect(&timer, SIGNAL(timeout()), this, SLOT(timerTimeoutSlot()));
	timer.start(1000);

	kdebugf2();
}

AutoHide::~AutoHide()
{
	kdebugf();
	kdebugf2();
}

void AutoHide::timerTimeoutSlot()
{
	if (config_file.readBoolEntry("PowerKadu", "auto_hide_use_auto_hide", false))
	{
		if (idle->secondsIdle() >= config_file.readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60))
		{
			KaduWindow *window = Core::instance()->kaduWindow();
			if (window->docked())
				window->hide();
		}
	}
}

void AutoHide::configurationUpdated()
{
	bool enabled = config_file.readBoolEntry("PowerKadu", "auto_hide_use_auto_hide");
	if (enabled && !timer.isActive())
		timer.start(1000);
	else if (!enabled && timer.isActive())
		timer.stop();
}

void AutoHide::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("auto_hide/use_auto_hide"), SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time"), SLOT(setEnabled(bool)));

	(dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("auto_hide/idle_time")))->setSpecialValueText(tr("Dont hide"));
}
