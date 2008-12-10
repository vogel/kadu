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
	qApp->installEventFilter(this);
	kdebugf2();
}

AutoHide::~AutoHide()
{
	kdebugf();
	kdebugf2();
}

bool AutoHide::eventFilter(QObject *o, QEvent *e)
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::Enter || e->type() == QEvent::MouseMove)
		idleTime = 0;
	return QObject::eventFilter(o, e);
}

void AutoHide::timerTimeoutSlot()
{
	//kdebugf();
	if(config_file.readBoolEntry("PowerKadu", "auto_hide_use_auto_hide", false))
	{
		idleTime++;
		
		// Copied from autoaway.cpp  in autoaway module
		const static int INTCOUNT=16;
		static unsigned long interrupts[INTCOUNT]={0};
		unsigned long currentInterrupts[INTCOUNT]={0};
	
		static QPoint MousePosition(0, 0);
		QPoint currentMousePosition;
	
		currentMousePosition = QCursor::pos();
		if (currentMousePosition != MousePosition)
			idleTime = 0;
		
		MousePosition = currentMousePosition;

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
					if (interrupt >= 0 && interrupt < INTCOUNT)
						currentInterrupts[interrupt] = strlist[1].toULong();
				}
			}
			f.close();
		
			if (memcmp(interrupts, currentInterrupts, INTCOUNT * sizeof(interrupts[0])) != 0)
			{
				idleTime = 0;
				memcpy(interrupts, currentInterrupts, INTCOUNT * sizeof(interrupts[0]));
			}
		}

		if(idleTime >= config_file.readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60))
			kadu->close();
	
		//kdebugm(KDEBUG_INFO, "idleTime: %d\n", idleTime);
		//kdebugm(KDEBUG_INFO, "Musi byc: %d\n", config_file.readNumEntry("PowerKadu", "auto_hide_idle_time", 5 * 60));
	}
	//kdebugf2();
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
