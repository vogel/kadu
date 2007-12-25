/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include <qapplication.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qcheckbox.h>
#include <qdesktopwidget.h>
#include <qpopupmenu.h>
#include <qspinbox.h>

#include "../docking/docking.h"

#include "config_file.h"
#include "debug.h"
#include "desktopdock.h"
#include "kadu.h"

/**
 * @ingroup desktop_docking
 * @{
 */
extern "C" int desktop_docking_init()
{
	kdebugf();

	desktop_dock = new DesktopDock();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/desktop_docking.ui"), desktop_dock);

	kdebugf2();
	return 0;
}

extern "C" void desktop_docking_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/desktop_docking.ui"), desktop_dock);
	delete desktop_dock;
	desktop_dock = 0;

	kdebugf2();
}

DesktopDockWindow::DesktopDockWindow(QWidget *parent, char *name)
	: QLabel(parent, name, WMouseNoMask | WRepaintNoErase | WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WX11BypassWM),
		isMoving(false)
{
	QPixmap desktopDockPixmap = docking_manager->defaultPixmap();
	configurationUpdated();
	setMouseTracking(true);

	setPixmap(desktopDockPixmap);
	resize(desktopDockPixmap.size());

	update();
	show();
}

DesktopDockWindow::~DesktopDockWindow()
{
}

void DesktopDockWindow::configurationUpdated()
{
	setAutoMask(config_file.readBoolEntry("Desktop Dock", "DockingTransparency"));
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));
	if (!config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
		setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));
   	move(pos);
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)
{
	if (!isMoving)
		docking_manager->trayMousePressEvent(ev);
	else
	{
		emit dropped(QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2));
		isMoving = false;
	}
}

void DesktopDockWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (isMoving)
		move(QPoint(ev->globalPos().x() - width() / 2, ev->globalPos().y() - height() / 2));
}

void DesktopDockWindow::updateMask()		/* to zalatwia automatyczne odswiezenie ikony - jak to dziala nie pytac (wazne ze dziala) */
{
	if (pixmap())
		setMask(pixmap()->createHeuristicMask(false));
}

void DesktopDockWindow::startMoving()	/* rozpoczynamy wojaze po ekranie */
{
	isMoving = true;
}

DesktopDock::DesktopDock()
	: menuPos(0), separatorPos(0)
{
	kdebugf();

	createDefaultConfiguration();

	desktopDock = new DesktopDockWindow(0, 0);

// fullDesktop->width() - DesktopDockPixmap.size().width(), 1, 0,
// fullDesktop->height() - DesktopDockPixmap.size().height(), 1, 0,

	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&, const QString &)), this,  SLOT(setPixmap(const QPixmap&, const QString &)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));

	connect(desktopDock, SIGNAL(dropped(const QPoint &)), this, SLOT(droppedOnDesktop(const QPoint &)));

	docking_manager->setDocked(true);

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		separatorPos = dockMenu->insertSeparator();
		menuPos = dockMenu->insertItem(tr("Move"), desktopDock, SLOT(startMoving()));
	}

	kdebugmf(KDEBUG_INFO, "Move's ID is = %d\n", menuPos);

	kdebugf2();
}

DesktopDock::~DesktopDock()
{
	kdebugf();

	disconnect(docking_manager, SIGNAL(trayMovieChanged(const QMovie &)), this, SLOT(setTrayMovie(const QMovie &)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&, const QString &)), this, SLOT(setPixmap(const QPixmap&, const QString &)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	docking_manager->setDocked(false);

	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		dockMenu->removeItem(menuPos);
		dockMenu->removeItem(separatorPos);
	}

	delete desktopDock;
	desktopDock = 0;

	kdebugf2();
}

void DesktopDock::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widgetById("desktop_docking/transparent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("desktop_docking/color"), SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow->widgetById("desktop_docking/move"), SIGNAL(clicked()), desktopDock, SLOT(startMoving()));

	xSpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("desktop_docking/x"));
	ySpinBox = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("desktop_docking/y"));
}

void DesktopDock::configurationUpdated()
{
	kdebugf();
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));

	desktopDock->move(pos);
	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
	{
		desktopDock->setAutoMask(true);
		desktopDock->close();
		desktopDock->show();
	}
	else
	{
		desktopDock->setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));	/* ustawia kolor tla */
		desktopDock->setAutoMask(false);
		desktopDock->repaint();
	}
	kdebugf2();
}

void DesktopDock::setToolTip(const QString& statusText)
{
	QToolTip::add(desktopDock, statusText);
}

void DesktopDock::setPixmap(const QPixmap& DockPixmap, const QString & /*iconName*/)
{
	desktopDock->setPixmap(DockPixmap);
	desktopDock->repaint();
	desktopDock->setMask(desktopDock->pixmap()->createHeuristicMask(false));
}

void DesktopDock::setTrayMovie(const QMovie &movie)
{
	desktopDock->setMovie(movie);
	desktopDock->repaint();
}

void DesktopDock::findTrayPosition(QPoint& DockPoint)	/* zwrocenie krawedzi ikony */
{
	DockPoint = desktopDock->mapToGlobal(QPoint(0,0));
}

void DesktopDock::droppedOnDesktop(const QPoint& pos) 	/* nacisniecie przycisku na ekranie - ustawienie ikonki i zapisanie ustawien */
{
	desktopDock->move(pos);
	desktopDock->update();
	desktopDock->show();

	QDesktopWidget *fullDesktop = QApplication::desktop();
	int posX, posY;

	if (pos.x() > fullDesktop->width() - desktopDock->pixmap()->width())
		posX = fullDesktop->width() - desktopDock->pixmap()->width();
	else
		posX = pos.x();

	if (pos.y() > fullDesktop->height() - desktopDock->pixmap()->height())
		posY = fullDesktop->height() - desktopDock->pixmap()->height();
	else
		posY = pos.y();

	config_file.writeEntry("Desktop Dock", "PositionX", posX);
	config_file.writeEntry("Desktop Dock", "PositionY", posY);
}

void DesktopDock::updateMenu(bool b)
{
	if (b)
	{
		separatorPos = dockMenu->insertSeparator();
		menuPos = dockMenu->insertItem(tr("Move"), desktopDock, SLOT(startMoving()));
	}
	else
	{
		dockMenu->removeItem(menuPos);
		dockMenu->removeItem(separatorPos);
	}
}

void DesktopDock::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Desktop Dock", "DockingColor", w.paletteBackgroundColor());
	config_file.addVariable("Desktop Dock", "DockingTransparency", true);
	config_file.addVariable("Desktop Dock", "MoveInMenu", true);
	config_file.addVariable("Desktop Dock", "PositionX", 0);
	config_file.addVariable("Desktop Dock", "PositionY", 0);
}

DesktopDock *desktop_dock;

/** }@ */
