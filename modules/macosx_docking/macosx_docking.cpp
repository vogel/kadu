/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "macosx_docking.h"
#include "../docking/docking.h"
#include "debug.h"
#include "kadu.h"

MacOSXDocking::MacOSXDocking(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap &, const QString &)), this, SLOT(trayPixmapChanged(const QPixmap &, const QString &)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint &)), this, SLOT(findTrayPosition(QPoint &)));
	connect(kadu, SIGNAL(settingMainIconBlocked(bool &)), this, SLOT(blockSettingIcon(bool &)));
	docking_manager->setDocked(true, true);
	kdebugf2();
}

MacOSXDocking::~MacOSXDocking()
{
	kdebugf();
	docking_manager->setDocked(false);
	disconnect(kadu, SIGNAL(settingMainIconBlocked(bool &)), this, SLOT(blockSettingIcon(bool &)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap &, const QString &)), this, SLOT(trayPixmapChanged(const QPixmap &, const QString &)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint &)), this, SLOT(findTrayPosition(QPoint &)));
	kdebugf2();
}

void MacOSXDocking::blockSettingIcon(bool &block)
{
	block = true;
}

void MacOSXDocking::trayPixmapChanged(const QPixmap &small_pix, const QString &name)
{
	const QPixmap &pix = icons_manager.loadIcon("Big" + name);
	if (pix.isNull())
	{
		kdebugm(KDEBUG_WARNING, "big icon of %s not available\n", name.local8Bit().data());
		kadu->setIcon(small_pix);
	}
	else
		kadu->setIcon(pix);
}

void MacOSXDocking::findTrayPosition(QPoint &p)
{
	QDesktopWidget *desktop = QApplication::desktop();
	p = QPoint(desktop->width() - 20, desktop->height() - 20);
}

extern "C" int macosx_docking_init()
{
	mac_docking = new MacOSXDocking(docking_manager, "mac_docking");
	return 0;
}

extern "C" void macosx_docking_close()
{
	delete mac_docking;
	mac_docking = NULL;
}

MacOSXDocking *mac_docking;
