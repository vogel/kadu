/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#include <qbitmap.h>
#include <qtooltip.h>

#include "desktopdock.h"
#include "config_file.h"
#include "config_dialog.h"
#include "../docking/docking.h"
#include "debug.h"

extern "C" int desktop_docking_init()
{
	kdebugf();
	DesktopDockObj=new DesktopDockWindow();
	ConfigDialog::registerSlotOnApply(DesktopDockObj, SLOT(ApplyConfig()));
	ConfigDialog::registerSlotOnCreate(DesktopDockObj, SLOT(onCreateConfigDialog()));

	kdebugf2();
	return 0;
}

extern "C" void desktop_docking_close()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnApply(DesktopDockObj, SLOT(ApplyConfig()));
	ConfigDialog::unregisterSlotOnCreate(DesktopDockObj, SLOT(onCreateConfigDialog()));
	delete DesktopDockObj;
	DesktopDockObj=NULL;

	kdebugf2();
}

DesktopDockWindow::DesktopDockWindow()
	: QLabel(0,"DesktopDockIcon", WMouseNoMask | WRepaintNoErase | WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WX11BypassWM)
{
	kdebugf();

	QPixmap DesktopDockPixmap=docking_manager->defaultPixmap();	/*ustawia ikonke na starcie - nie uzywam bo moze byc jak z defaultToolTip()*/
	QDesktopWidget *fullDesktop = QApplication::desktop();	/* ekran */

	ConfigDialog::addTab("Desktop Dock");
	ConfigDialog::addVBox("Desktop Dock", "Desktop Dock", "MainDesktop");
	ConfigDialog::addSpinBox("Desktop Dock", "MainDesktop", QT_TRANSLATE_NOOP("@default", "Horizontal position"), "PositionX", 0, fullDesktop->width()-DesktopDockPixmap.size().width(), 1, 0,
        QT_TRANSLATE_NOOP("@default", "Put the position in pixels"));
	ConfigDialog::addSpinBox("Desktop Dock", "MainDesktop", QT_TRANSLATE_NOOP("@default", "Vertical position"), "PositionY", 0, fullDesktop->height()-DesktopDockPixmap.size().height(), 1, 0,
        QT_TRANSLATE_NOOP("@default", "Put the position in pixels"));
	ConfigDialog::addHBox("Desktop Dock", "Desktop Dock", "Colors");
	ConfigDialog::addColorButton( "Desktop Dock", "Colors", "Background color", "DockingColor", QColor("black"));
	ConfigDialog::addCheckBox("Desktop Dock", "Colors", QT_TRANSLATE_NOOP("@default", "Transparent"), "DockingTransparency", false);
	
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
	kdebugf2();
}

DesktopDockWindow::~DesktopDockWindow()
{
	kdebugf();
	
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setToolTip(const QString&)));
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&)), this, SLOT(setPixmap(const QPixmap&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	ConfigDialog::removeControl("Desktop Dock", "Horizontal position");
	ConfigDialog::removeControl("Desktop Dock", "Vertical position");
	ConfigDialog::removeControl("Desktop Dock", "Background color");
	ConfigDialog::removeControl("Desktop Dock", "Transparent");
	ConfigDialog::removeControl("Desktop Dock", "Colors");
	ConfigDialog::removeControl("Desktop Dock", "MainDesktop");
	ConfigDialog::removeTab("Desktop Dock");	
	
	docking_manager->setDocked(false);
	kdebugf2();
}

void DesktopDockWindow::mousePressEvent(QMouseEvent *ev)	/* przekazanie klikniecia mysza */
{
	docking_manager->trayMousePressEvent(ev);
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

DesktopDockWindow *DesktopDockObj;
