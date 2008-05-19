/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include <QApplication>
#include <QBitmap>
#include <QToolTip>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QMenu>
#include <QSpinBox>

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
	: QLabel(parent, name, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint),
		isMoving(false)
{
	setAttribute(Qt::WA_NoBackground);
	setAttribute(Qt::WA_MouseNoMask);

	QIcon desktopDockIcon = docking_manager->defaultPixmap();
	configurationUpdated();
	setMouseTracking(true);

	setPixmap(desktopDockIcon.pixmap(128, 128));
	resize(pixmap()->size());

	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
		setPaletteBackgroundColor(Qt::transparent);
	else
		setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));

	update();
	show();
}

DesktopDockWindow::~DesktopDockWindow()
{
}

void DesktopDockWindow::configurationUpdated()
{
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
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this,  SLOT(setPixmap(const QIcon&, const QString &)));
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
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QIcon&, const QString &)), this, SLOT(setPixmap(const QIcon&, const QString &)));
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
		desktopDock->setPaletteBackgroundColor(Qt::transparent); /* ustawia kolor tla */
		desktopDock->close();
		desktopDock->show();
	}
	else
	{
		desktopDock->setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));	/* ustawia kolor tla */
		desktopDock->repaint();
	}
	kdebugf2();
}

void DesktopDock::setToolTip(const QString& statusText)
{
	desktopDock->setTooltip(statusText);
}

void DesktopDock::setPixmap(const QIcon& DockIcon, const QString & /*iconName*/)
{
	desktopDock->setPixmap(DockIcon.pixmap(128,128));
	desktopDock->repaint();
	desktopDock->setMask(desktopDock->pixmap()->createHeuristicMask(false));
}

void DesktopDock::setTrayMovie(const QMovie &movie)
{
	desktopDock->setMovie((QMovie *)&movie);
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

	xSpinBox->setValue(posX);
	ySpinBox->setValue(posY);
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
