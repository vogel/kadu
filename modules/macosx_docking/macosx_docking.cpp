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
#include <QtGui/QDesktopWidget>

#include "macosx_docking.h"
#include "../docking/docking.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

/**
 * @ingroup macosx_docking
 * @{
 */
MacOSXDocking::MacOSXDocking(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	config_file.writeEntry("General", "RunDocked", false);

	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon &, const QString &)), this, SLOT(trayPixmapChanged(const QIcon &, const QString &)));
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
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon &, const QString &)), this, SLOT(trayPixmapChanged(const QIcon &, const QString &)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint &)), this, SLOT(findTrayPosition(QPoint &)));

	kdebugf2();
}

void MacOSXDocking::blockSettingIcon(bool &block)
{
	block = true;
}

void MacOSXDocking::trayPixmapChanged(const QIcon &small_pix, const QString &name)
{
	if (!config_file.readBoolEntry("MacOSX Dock", "IconNotification", true))
		return;

	const QIcon &pix = icons_manager->loadIcon("Big" + name);
	if (pix.isNull())
	{
		kdebugm(KDEBUG_WARNING, "big icon of %s not available\n", qPrintable(name));
		kadu->setIcon(small_pix.pixmap(128,128));
		qApp->setWindowIcon(small_pix.pixmap(128,128));
	}
	else
	{
		kadu->setIcon(pix.pixmap(128,128));
		qApp->setWindowIcon(pix.pixmap(128,128));
	}
}

void MacOSXDocking::findTrayPosition(QPoint &p)
{
	QDesktopWidget *desktop = QApplication::desktop();
	p = QPoint(desktop->width() - 20, desktop->height() - 20);
}

void MacOSXDocking::onCreateTabGeneral()
{
	kdebugf();
	//ze wzgl�du na jaki� problem z Qt opcja wy��czona
	//(okno pojawia si�, znika i znowu pojawia, wi�c nie do��, �e nie dzia�a,
	//  to mo�e by� denerwuj�ca je�eli kto� zapomnia�, �e to w��czy�)
	config_file.writeEntry("General", "RunDocked", false);
}

void MacOSXDocking::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	//w tej chwili nic tu nie robimy, ale kto wie ;)
}

extern "C" int macosx_docking_init(bool firstLoad)
{
	mac_docking = new MacOSXDocking(docking_manager, "mac_docking");
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/macosx_docking.ui"), mac_docking);
	return 0;
}

extern "C" void macosx_docking_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/macosx_docking.ui"), mac_docking);
	delete mac_docking;
	mac_docking = NULL;
}

MacOSXDocking *mac_docking;

/** @} */
