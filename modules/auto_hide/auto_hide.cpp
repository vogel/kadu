/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QCursor>
#include <QtCore/QFile>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtCore/QTextStream>

#include "auto_hide.h"

#include "config_file.h"
#include "kadu.h"
#include "debug.h"
#include "../idle/idle.h"

AutoHide *autoHide;

extern "C" KADU_EXPORT int auto_hide_init()
{
	kdebugf();

	autoHide = new AutoHide();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/auto_hide.ui"), autoHide);

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void auto_hide_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/auto_hide.ui"), autoHide);
	delete autoHide;
	autoHide = NULL;

	kdebugf2();
}


AutoHide::AutoHide(QObject *parent, const char *name)
: QObject(parent, name), idleTime(0)
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
	if(config_file.readBoolEntry("PowerKadu", "auto_hide_use_auto_hide", false))
	{
		if (idle->secondsIdle() >= config_file.readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60))
			kadu->close();
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
	connect(mainConfigurationWindow->widgetById("auto_hide/use_auto_hide"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("auto_hide/idle_time"), SLOT(setEnabled(bool)));

	(dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("auto_hide/idle_time")))->setSpecialValueText(tr("Dont hide"));
}
