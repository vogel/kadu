/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "docking.h"

#include <qapplication.h>
#include <qcursor.h>
#include <qobject.h>
#include <qtimer.h>

#include "config_file.h"
#include "config_dialog.h"
#include "debug.h"
#include "pending_msgs.h"
#include "status.h"
#include "kadu.h"
#include "hints.h"

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

extern "C" int docking_init()
{
	trayicon = new TrayIcon(kadu);
	trayicon->show();
	QPixmap pix=icons_manager.loadIcon(gg_icons[
		statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
	trayicon->setType(pix);
	trayicon->changeIcon();
	trayicon->connectSignals();
	return 0;
}

extern "C" int docking_close()
{
	delete trayicon;
	trayicon = NULL;
}

TrayIcon::TrayIcon(QWidget *parent, const char *name)
	: QLabel(0,"TrayIcon", WMouseNoMask | WRepaintNoErase | WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop)
{
	QPixmap pix = icons_manager.loadIcon("Offline");
	setBackgroundMode(X11ParentRelative);
	setMinimumSize(pix.size());
	QLabel::setPixmap(pix);
	resize(pix.size());
	setMouseTracking(true);
	QToolTip::add(this, tr("Left click - hide/show window\nMiddle click or Left click- next message"));
	update();

	icon_timer = new QTimer(this);
	blink = FALSE;
	QObject::connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	Display *dsp = x11Display();
	WId win = winId();

	// Okno potrzebne dla WindowMakera, ale przydaje
	// siê te¿ w KDE 3.1, gdzie je¶li chocia¿ na chwilê
	// nie poka¿emy okna g³ównego na ekranie wystêpuje
	// "efekt klepsydry"
	WMakerMasterWidget=new QWidget(0,"WMakerMasterWidget");
	WMakerMasterWidget->setGeometry(-10,-10,0,0);

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
	int r1=XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
	r = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
	int r2=XChangeProperty(dsp, win, r, XA_WINDOW, 32, 0, (uchar *)&data, 1);
			
	// SPOSÓB TRZECI
	// Dzia³a pod Window Maker'em
	WId w_id = WMakerMasterWidget->winId();
	XWMHints *hints;
	hints = XGetWMHints(dsp, w_id);
	hints->icon_window = win;
	hints->window_group = w_id;
	hints->flags |= WindowGroupHint | IconWindowHint;
	XSetWMHints(dsp, w_id, hints);
	XFree( hints );

	QT_TRANSLATE_NOOP("@default", "Start docked");
	ConfigDialog::addCheckBox("General", "grid", "Start docked", "RunDocked", false);
	if (config_file.readBoolEntry("General", "RunDocked"))
		kadu->hide();

	connect(kadu, SIGNAL(connectingBlinkShowOffline()), this, SLOT(showOffline()));
	connect(kadu, SIGNAL(connectingBlinkShowStatus(int)), this, SLOT(showStatus(int)));
	connect(kadu, SIGNAL(currentStatusChanged(int)), this, SLOT(showCurrentStatus(int)));
	connect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	hintmanager->setDetectedPosition(trayPosition());
}

TrayIcon::~TrayIcon()
{
	kdebug("TrayIcon::~TrayIcon()\n");

	ConfigDialog::removeControl("grid", "Start docked");

	disconnect(kadu, SIGNAL(connectingBlinkShowOffline()), this, SLOT(showOffline()));
	disconnect(kadu, SIGNAL(connectingBlinkShowStatus(int)), this, SLOT(showStatus(int)));
	disconnect(kadu, SIGNAL(currentStatusChanged(int)), this, SLOT(showCurrentStatus(int)));
	disconnect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	delete WMakerMasterWidget;
}

QPoint TrayIcon::trayPosition()
{
	return mapToGlobal(QPoint(0,0));
}

void TrayIcon::show()
{
	QLabel::show();
	WMakerMasterWidget->show();
	// Je¶li WindowMaker nie jest aktywny okno
	// nie powinno zostaæ widoczne
	if(XInternAtom(x11Display(),"_WINDOWMAKER_WM_PROTOCOLS",true)==0)
		WMakerMasterWidget->hide();
}

void TrayIcon::setPixmap(const QPixmap& pixmap)
{
	QLabel::setPixmap(pixmap);
	WMakerMasterWidget->setIcon(pixmap);
	repaint();
}

void TrayIcon::setType(const QPixmap& pixmap)
{
	setPixmap(pixmap);
}

void TrayIcon::changeIcon() {
	if (pending.pendingMsgs() && !icon_timer->isActive()) {
		if (!blink) {
			setPixmap(icons_manager.loadIcon("Message"));
			icon_timer->start(500,TRUE);
			blink = true;
			}
		else {
			setPixmap(icons_manager.loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
			icon_timer->start(500,TRUE);
			blink = false;
			}
	}
	else {
		kdebug("DockWidget::changeIcon() OFF\n");
		}
}

void TrayIcon::dockletChange(int id)
{
	if (id < 9)
		kadu->slotHandleState(id);
	else
		kadu->close(true);
}

void TrayIcon::connectSignals() {
	QObject::connect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
}

void TrayIcon::resizeEvent(QResizeEvent* e)
{
//	if (icon)
//		icon->resize(size());
//	resize(size());
}

void TrayIcon::enterEvent(QEvent* e)
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

void TrayIcon::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == MidButton) {
		emit mousePressMidButton();
		return;
	}

	if (e->button() == LeftButton) {
		emit mousePressLeftButton();
		if (pending.pendingMsgs() && e->state() != ControlButton) {
			pending.openMessages();
			return;
			}
		switch (kadu->isVisible()) {
			case 0:
				kadu->show();
				kadu->setFocus();
				break;
			case 1:
				kadu->hide();
				break;
			}
		return;
		}

	if (e->button() == RightButton) {
		emit mousePressRightButton();
		dockppm->exec(QCursor::pos());
		return;
		}
}

void TrayIcon::pendingMessageAdded()
{
	changeIcon();
}

void TrayIcon::pendingMessageDeleted()
{
	if (!pending.pendingMsgs())
	{
		QPixmap pix= icons_manager.loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
		setType(pix);
	}
}

void TrayIcon::showOffline()
{
	setType(icons_manager.loadIcon("Offline"));
}

void TrayIcon::showStatus(int status)
{
	int i = statusGGToStatusNr(status);
	setType(icons_manager.loadIcon(gg_icons[i]));
}

void TrayIcon::showCurrentStatus(int status)
{
	int statusnr = statusGGToStatusNr(status);
	QPixmap pix = icons_manager.loadIcon(gg_icons[statusnr]);
	if (!pending.pendingMsgs())
		setType(pix);
}

TrayIcon *trayicon = NULL;
