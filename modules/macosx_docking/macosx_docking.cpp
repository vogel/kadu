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
#include <QtCore/QTimer>

#include "ApplicationServices/ApplicationServices.h"
#include "Carbon/Carbon.h"

#include "macosx_docking.h"
#include "../docking/docking.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "pending_msgs.h"
#include "userlist.h"

#define DOCK_FONT_NAME "LucidaGrande-Bold"
#define DOCK_FONT_SIZE 24
	
static NMRec bounceRec;

/**
 * @ingroup macosx_docking
 * @{
 */
MacOSXDocking::MacOSXDocking(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	//config_file.writeEntry("General", "RunDocked", false);

	isBouncing = false;
	overlayed = false;

	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon &, const QString &)), this, SLOT(trayPixmapChanged(const QIcon &, const QString &)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint &)), this, SLOT(findTrayPosition(QPoint &)));
	connect(kadu, SIGNAL(settingMainIconBlocked(bool &)), this, SLOT(blockSettingIcon(bool &)));

	connect(&pending, SIGNAL(messageFromUserAdded(UserListElement)), this, SLOT(messageListChanged(UserListElement)));
	connect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageListChanged(UserListElement)));

	docking_manager->setDocked(true);
	
	
	kdebugf2();
}

MacOSXDocking::~MacOSXDocking()
{
	kdebugf();
	docking_manager->setDocked(false);

	disconnect(kadu, SIGNAL(settingMainIconBlocked(bool &)), this, SLOT(blockSettingIcon(bool &)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon &, const QString &)), this, SLOT(trayPixmapChanged(const QIcon &, const QString &)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint &)), this, SLOT(findTrayPosition(QPoint &)));

	disconnect(&pending, SIGNAL(messageFromUserAdded(UserListElement)), this, SLOT(messageListChanged(UserListElement)));
	disconnect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(messageListChanged(UserListElement)));

	kdebugf2();
}

void MacOSXDocking::toggleKaduVisibility()
{
	if (!kadu->isVisible())
	{
		kadu->show();
		kadu->raise();
	}
	else
		kadu->hide();
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
		pixmap = small_pix.pixmap(128,128);
	}
	else
	{
		pixmap = pix.pixmap(128,128);
	}

	kadu->setIcon(pixmap);
	qApp->setWindowIcon(pixmap);
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
	//config_file.writeEntry("General", "RunDocked", false);
}

void MacOSXDocking::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	//w tej chwili nic tu nie robimy, ale kto wie ;)
}

void MacOSXDocking::messageListChanged(UserListElement ule)
{
	int num = pending.count();
	if (num > 0)
	{
		if (config_file.readBoolEntry("MacOSX Dock", "ShowMessgeNum", true))
			overlay(QString::number(pending.count()));
		if (config_file.readBoolEntry("MacOSX Dock", "BounceIcon", true))
			startBounce();
	}
	else
	{
		if (overlayed)
			overlay("");
		if (isBouncing)
			stopBounce();
	}	
}

void MacOSXDocking::startBounce()
{
	/* The following code is taken from PSI mac_dock sources */
	if (!isBouncing) {
		bounceRec.qType = nmType;
		bounceRec.nmMark = 1;
		bounceRec.nmIcon = NULL;
		bounceRec.nmSound = NULL;
		bounceRec.nmStr = NULL;
		bounceRec.nmResp = NULL;
		bounceRec.nmRefCon = 0;
		NMInstall(&bounceRec);
		isBouncing = true;
	}
}

void MacOSXDocking::stopBounce()
{
	/* The following code is taken from PSI mac_dock sources */
	if(isBouncing) {
		NMRemove(&bounceRec);
		isBouncing = false;
	}
}

void MacOSXDocking::overlay(const QString& text)
{
	/* The following code is taken from PSI mac_dock sources */

	// Create the context
	if (text.isEmpty()) {
		overlayed = false;
		RestoreApplicationDockTileImage();
		qApp->setWindowIcon(pixmap);
		return;
	}

	CGContextRef context = BeginCGContextForApplicationDockTile();

	if (!overlayed) {
		overlayed = true;
		// Add some subtle drop down shadow
		//CGSize s = { 2.0, -4.0 };
		//CGContextSetShadow(context, s, 5.0);
	}

	// Draw a circle
	CGContextBeginPath(context);
	CGContextAddArc(context, 95.0, 95.0, 25.0, 0.0, 2 * M_PI, true);
	CGContextClosePath(context);
	CGContextSetRGBFillColor(context, 1, 0.0, 0.0, 1);
	CGContextFillPath(context);

	// Set the clipping path to the same circle
	CGContextBeginPath(context);
	CGContextAddArc(context, 95.0, 95.0, 25.0, 0.0, 2 * M_PI, true);
	CGContextClip(context);

	// Select the appropriate font
	CGContextSelectFont(context,DOCK_FONT_NAME, DOCK_FONT_SIZE, kCGEncodingMacRoman);
	CGContextSetRGBFillColor(context, 1, 1, 1, 1);

	// Draw the text invisible
	CGPoint begin = CGContextGetTextPosition(context);
	CGContextSetTextDrawingMode(context, kCGTextInvisible);	
	CGContextShowTextAtPoint(context, begin.x, begin.y, text.toStdString().c_str(), text.length());
	CGPoint end = CGContextGetTextPosition(context);

	// Draw the text
	CGContextSetTextDrawingMode(context, kCGTextFill);	
	CGContextShowTextAtPoint(context, 95 - (end.x - begin.x)/2, 95 - 8, text.toStdString().c_str(), text.length());
	
	// Cleanup
	CGContextFlush(context);
	EndCGContextForApplicationDockTile(context);
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
