/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include <qbitmap.h>
#include <qtooltip.h>
#include <qcheckbox.h>

#include "desktopdock.h"
#include "config_file.h"
#include "config_dialog.h"
#include "../docking/docking.h"
#include "debug.h"
#include "kadu.h"

extern "C" int desktop_docking_init()
{
	kdebugf();
	desktop_dock_window=new DesktopDockWindow(NULL, "desktop_dock_window");
	ConfigDialog::registerSlotOnApply(desktop_dock_window, SLOT(ApplyConfig()));
	ConfigDialog::registerSlotOnCreate(desktop_dock_window, SLOT(onCreateConfigDialog()));

	kdebugf2();
	return 0;
}

extern "C" void desktop_docking_close()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnApply(desktop_dock_window, SLOT(ApplyConfig()));
	ConfigDialog::unregisterSlotOnCreate(desktop_dock_window, SLOT(onCreateConfigDialog()));
	delete desktop_dock_window;
	desktop_dock_window=NULL;

	kdebugf2();
}

DesktopDockWindow::DesktopDockWindow(QWidget *parent, const char *name)
	: QLabel(parent, name, WMouseNoMask | WRepaintNoErase | WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WX11BypassWM)
{
	kdebugf();
	isMoving = false;
	QPixmap DesktopDockPixmap=docking_manager->defaultPixmap();	/*ustawia ikonke na starcie - nie uzywam bo moze byc jak z defaultToolTip()*/
	QDesktopWidget *fullDesktop = QApplication::desktop();	/* ekran */

	ConfigDialog::addTab("Desktop Dock", "DesktopDockTab");
	ConfigDialog::addVBox("Desktop Dock", "Desktop Dock", "MainDesktop");
	ConfigDialog::addSpinBox("Desktop Dock", "MainDesktop", QT_TRANSLATE_NOOP("@default", "Horizontal position"), "PositionX", 0, fullDesktop->width() - DesktopDockPixmap.size().width(), 1, 0,
        QT_TRANSLATE_NOOP("@default", "Put the position in pixels"));
	ConfigDialog::addSpinBox("Desktop Dock", "MainDesktop", QT_TRANSLATE_NOOP("@default", "Vertical position"), "PositionY", 0, fullDesktop->height() - DesktopDockPixmap.size().height(), 1, 0,
        QT_TRANSLATE_NOOP("@default", "Put the position in pixels"));
	ConfigDialog::addHBox("Desktop Dock", "Desktop Dock", "Colors");
	ConfigDialog::addColorButton( "Desktop Dock", "Colors", QT_TRANSLATE_NOOP("@default", "Background color"), "DockingColor", QColor("black"));
	ConfigDialog::addCheckBox("Desktop Dock", "Colors", QT_TRANSLATE_NOOP("@default", "Transparent"), "DockingTransparency", false);
	
	ConfigDialog::addCheckBox("Desktop Dock", "Desktop Dock", QT_TRANSLATE_NOOP("@default", "Enable Move entry in docklet's menu"), "MoveInMenu", true);
	ConfigDialog::connectSlot("Desktop Dock", "Enable Move entry in docklet's menu", SIGNAL(toggled(bool)), this, SLOT(updateMenu(bool)));
	
	ConfigDialog::addPushButton("Desktop Dock", "Desktop Dock", QT_TRANSLATE_NOOP("@default", "Move"), "", QT_TRANSLATE_NOOP("@default", "Enable icon moving on desktop. After pressing move cursor over docking icon and then move. Press any mouse key when the icon is in right place."));
	
	ConfigDialog::connectSlot("Desktop Dock", "Move", SIGNAL(clicked()), this, SLOT(startMoving()));
	
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY")); /* wpisuje pozycje ikonki */ 
	resize(DesktopDockPixmap.size());	/* ustawia rozmiar okna na taki jaki ma ikonka */

	if (!config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
		QLabel::setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));	/* ustawia kolor tla */
	
	QLabel::setPixmap(DesktopDockPixmap);	/* wrzuca ikonke*/
   	move(pos);	/*przesuwa sie na odpowiednia pozycje*/
	update();
	
	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&)), this, SLOT(setPixmap(const QPixmap&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	setMouseTracking(true);

	show();		/*wyswietla ikonke*/
	docking_manager->setDocked(true);
	setAutoMask(config_file.readBoolEntry("Desktop Dock", "DockingTransparency"));
	
	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		separatorPos = dockMenu->insertSeparator();
		menuPos = dockMenu->insertItem(tr("Move"), this, SLOT(startMoving()));
	}
	
	kdebugmf(KDEBUG_INFO, "Move's ID is = %d\n", menuPos);
	
	kdebugf2();
}

DesktopDockWindow::~DesktopDockWindow()
{
	kdebugf();
	
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&)), this, SLOT(setPixmap(const QPixmap&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	ConfigDialog::disconnectSlot("Desktop Dock", "Move", SIGNAL(clicked()), this, SLOT(startMoving()));
	ConfigDialog::disconnectSlot("Desktop Dock", "Enable Move entry in docklet's menu", SIGNAL(toggled(bool)), this, SLOT(updateMenu(bool)));

	ConfigDialog::removeControl("Desktop Dock", "Move");
	ConfigDialog::removeControl("Desktop Dock", "Enable Move entry in docklet's menu");
	ConfigDialog::removeControl("Desktop Dock", "Horizontal position");
	ConfigDialog::removeControl("Desktop Dock", "Vertical position");
	ConfigDialog::removeControl("Desktop Dock", "Background color");
	ConfigDialog::removeControl("Desktop Dock", "Transparent");
	ConfigDialog::removeControl("Desktop Dock", "Colors");
	ConfigDialog::removeControl("Desktop Dock", "MainDesktop");
	ConfigDialog::removeTab("Desktop Dock");	
	
	docking_manager->setDocked(false);
	
	if (config_file.readBoolEntry("Desktop Dock", "MoveInMenu"))
	{
		dockMenu->removeItem(menuPos);
		dockMenu->removeItem(separatorPos);
	}
	
	kdebugf2();
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)	/* przekazanie klikniecia mysza */
{
	if (!isMoving)
		docking_manager->trayMousePressEvent(ev);
	else 
	{
		droppedOnDesktop(QPoint(ev->globalPos().x() - width()/2, ev->globalPos().y() - height()/2));
		isMoving = false;
	}		
}

void DesktopDockWindow::mouseMoveEvent(QMouseEvent *ev)
{
	if (isMoving)
		move(QPoint(ev->globalPos().x() - width()/2, ev->globalPos().y() - height()/2));
}

void DesktopDockWindow::ApplyConfig()		/* przy zapisie konfiguracji */
{
	kdebugf();
	QPoint pos(config_file.readNumEntry("Desktop Dock", "PositionX"), config_file.readNumEntry("Desktop Dock", "PositionY"));
   	move(pos);
	if (config_file.readBoolEntry("Desktop Dock", "DockingTransparency"))
	{
		setAutoMask(true);	/* to daje przezroczystosc */
		close();
		show();
	}
	else 
	{
		QLabel::setPaletteBackgroundColor(config_file.readColorEntry("Desktop Dock", "DockingColor"));	/* ustawia kolor tla */
		setAutoMask(false);
		repaint();
	}
	kdebugf2();
}

void DesktopDockWindow::onCreateConfigDialog()
{
	kdebugf();
	ConfigDialog::getColorButton("Desktop Dock", "Background color")
		->setEnabled(!config_file.readBoolEntry("Desktop Dock", "DockingTransparency"));

	QCheckBox *b_transparent=ConfigDialog::getCheckBox("Desktop Dock", "Transparent");	/* podpiecie pod zmiane CheckBoxa */
	connect(b_transparent, SIGNAL(toggled(bool)), this, SLOT(enableColorButton(bool)));
	kdebugf2();
}

void DesktopDockWindow::setToolTip(const QString& statusText)
{
	QToolTip::add(this, statusText);
}

void DesktopDockWindow::setPixmap(const QPixmap& DockPixmap)
{
	QLabel::setPixmap(DockPixmap);
	repaint();
	setMask(pixmap()->createHeuristicMask(false));
}

void DesktopDockWindow::findTrayPosition(QPoint& DockPoint)	/* zwrocenie krawedzi ikony */
{
	DockPoint=mapToGlobal(QPoint(0,0));
}

void DesktopDockWindow::enableColorButton(bool b)
{
	ConfigDialog::getColorButton("Desktop Dock", "Background color")->setEnabled(!b);
}

void DesktopDockWindow::updateMask()		/* to zalatwia automatyczne odswiezenie ikony - jak to dziala nie pytac (wazne ze dziala) */
{
	setMask(pixmap()->createHeuristicMask(false));
}

void DesktopDockWindow::droppedOnDesktop(const QPoint& pos) 	/* nacisniecie przycisku na ekranie - ustawienie ikonki i zapisanie ustawien */
{	
	move(pos);
	update();
	show();
	QDesktopWidget *fullDesktop = QApplication::desktop();
	
	if (pos.x() > fullDesktop->width() - pixmap()->width())
		config_file.writeEntry("Desktop Dock", "PositionX", fullDesktop->width() - pixmap()->width());
	else
		config_file.writeEntry("Desktop Dock", "PositionX", pos.x());
	
	if (pos.y() > fullDesktop->height() - pixmap()->height())
		config_file.writeEntry("Desktop Dock", "PositionY", fullDesktop->height() - pixmap()->height());
	else	
		config_file.writeEntry("Desktop Dock", "PositionY", pos.y());
}

void DesktopDockWindow::startMoving()	/* rozpoczynamy wojaze po ekranie */
{
	isMoving = true;
	
}

void DesktopDockWindow::updateMenu(bool b)
{
	if (b)
	{	
		separatorPos = dockMenu->insertSeparator();
		menuPos = dockMenu->insertItem(tr("Move"), this, SLOT(startMoving()));
	}
	else
	{
		dockMenu->removeItem(menuPos);
		dockMenu->removeItem(separatorPos);
	}
}

DesktopDockWindow *desktop_dock_window;
