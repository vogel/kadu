/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "x11_docking.h"

#include <qapplication.h>
#include <qtooltip.h>
#include <qobject.h>

#include "../docking/docking.h"
#include "debug.h"
#include "config_file.h"
#include "kadu.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

extern Time qt_x_time;

static XErrorHandler old_handler = 0;
static int dock_xerror = 0;

static int dock_xerrhandler(Display* dpy, XErrorEvent* err)
{
	dock_xerror = err->error_code;
	return old_handler(dpy, err);
}

static void trap_errors()
{
	dock_xerror = 0;
	old_handler = XSetErrorHandler(dock_xerrhandler);
}

static bool untrap_errors()
{
	XSetErrorHandler(old_handler);
	return (dock_xerror == 0);
}

static bool send_message(
	Display* dpy, /* display */
	Window w,     /* sender (tray icon window) */
	long message, /* message opcode */
	long data1,   /* message data 1 */
	long data2,   /* message data 2 */
	long data3    /* message data 3 */
)
{
	XEvent ev;

	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = w;
	ev.xclient.message_type = XInternAtom (dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = message;
	ev.xclient.data.l[2] = data1;
	ev.xclient.data.l[3] = data2;
	ev.xclient.data.l[4] = data3;

	trap_errors();
	XSendEvent(dpy, w, False, NoEventMask, &ev);
	XSync(dpy, False);
	return untrap_errors();
}

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

extern "C" int x11_docking_init()
{
	x11_tray_icon = new X11TrayIcon();
	return 0;
}

extern "C" void x11_docking_close()
{
	delete x11_tray_icon;
	x11_tray_icon = NULL;
}

X11TrayIcon::X11TrayIcon()
	: QLabel(0,"X11TrayIcon", WMouseNoMask | WRepaintNoErase | WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop)
{
	setBackgroundMode(X11ParentRelative);
	QPixmap pix = docking_manager->defaultPixmap();
	setMinimumSize(pix.size());
	QLabel::setPixmap(pix);
	resize(pix.size());
	setMouseTracking(true);
	update();

	Display *dsp = x11Display();
	WId win = winId();
	
	//unikamy efektu klepsydry w KDE
	QWidget *w=new QWidget();
	w->setGeometry(-100,-100,10,10);
	w->show();
	w->hide();
	delete w;

	// SPOSÓB PIERWSZY
	// System Tray Protocol Specification
	// Dzia³a pod KDE 3.1 i GNOME 2.x
	Screen *screen = XDefaultScreenOfDisplay(dsp);
	int screen_id = XScreenNumberOfScreen(screen);
	char buf[32];
	snprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen_id);
	Atom selection_atom = XInternAtom(dsp, buf, false);
	XGrabServer(dsp);
	Window manager_window = XGetSelectionOwner(dsp, selection_atom);
	if (manager_window != None)
		XSelectInput(dsp, manager_window, StructureNotifyMask);
	XUngrabServer(dsp);
	XFlush(dsp);
	if (manager_window != None)
		send_message(dsp, manager_window, SYSTEM_TRAY_REQUEST_DOCK, win, 0, 0);
	
	// SPOSÓB DRUGI
	// Dzia³a na KDE 3.0.x i pewnie na starszych
	// oraz pod GNOME 1.x
	int r;
	int data = 1;
	r = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
	/*int r1=*/XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
	r = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
	/*int r2=*/XChangeProperty(dsp, win, r, XA_WINDOW, 32, 0, (uchar *)&data, 1);
			
	connect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&)), this, SLOT(setTrayPixmap(const QPixmap&)));
	connect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	connect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));

	if (config_file.readBoolEntry("General", "RunDocked"))
		kadu->showMainWindowOnStart=false;
	show();
}

X11TrayIcon::~X11TrayIcon()
{
	kdebugf();
	disconnect(docking_manager, SIGNAL(trayPixmapChanged(const QPixmap&)), this, SLOT(setTrayPixmap(const QPixmap&)));
	disconnect(docking_manager, SIGNAL(trayTooltipChanged(const QString&)), this, SLOT(setTrayTooltip(const QString&)));
	disconnect(docking_manager, SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	kadu->show();
}

void X11TrayIcon::findTrayPosition(QPoint& pos)
{
	pos = mapToGlobal(QPoint(0,0));
}

void X11TrayIcon::show()
{
	QLabel::show();
}

void X11TrayIcon::setTrayPixmap(const QPixmap& pixmap)
{
	QLabel::setPixmap(pixmap);
	repaint();
}

void X11TrayIcon::setTrayTooltip(const QString& tooltip)
{
	QToolTip::add(this,tooltip);
}

void X11TrayIcon::enterEvent(QEvent* e)
{
	if (!qApp->focusWidget())
	{
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xfocus.display = qt_xdisplay();
		ev.xfocus.type = FocusIn;
		ev.xfocus.window = winId();
		ev.xfocus.mode = NotifyNormal;
		ev.xfocus.detail = NotifyAncestor;
		Time time = qt_x_time;
		qt_x_time = 1;
		qApp->x11ProcessEvent( &ev );
		qt_x_time = time;
	}
	QWidget::enterEvent(e);
}

void X11TrayIcon::mousePressEvent(QMouseEvent * e)
{
	docking_manager->trayMousePressEvent(e);
}

X11TrayIcon* x11_tray_icon = NULL;
