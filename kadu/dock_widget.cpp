/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <qobject.h>
#include <qpopupmenu.h>
#include <qpixmap.h>

#include "dock_widget.h"
#include "misc.h"
#include "pixmaps.h"
#include "message.h"
#include "chat.h"
#include "debug.h"

//
#include "kadu.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

void send_manager_message (	Display* d,
				    long         message,
				    Window       window,
				    long         data1,
				    long         data2,
				    long         data3)
{
  XClientMessageEvent ev;
  
  ev.type = ClientMessage;
  ev.window = window;
  ev.message_type = XInternAtom(d,"_NET_SYSTEM_TRAY_OPCODE",False); //icon->system_tray_opcode_atom;
  ev.format = 32;
  ev.data.l[0] = CurrentTime; //time(NULL); //gdk_x11_get_server_time (GTK_WIDGET (icon)->window);
  ev.data.l[1] = message;
  ev.data.l[2] = data1;
  ev.data.l[3] = data2;
  ev.data.l[4] = data3;

  
  //gdk_error_trap_push ();
  XSendEvent (d,window
	      /*icon->manager_window*/, False, NoEventMask, (XEvent *)&ev);
  XSync (d, False);
  //gdk_error_trap_pop ();
}

void send_dock_request (Display* d,QWidget *widget)
{
  send_manager_message (/*icon,*/d,
				      /*SYSTEM_TRAY_REQUEST_DOCK*/0,
				      widget->winId()/*icon->manager_window*/,
				      widget->winId()/*gtk_plug_get_id (GTK_PLUG (icon))*/,
				      0, 0);
}


TrayIcon::TrayIcon(QWidget *parent, const char *name)
	: QLabel(0,"TrayIcon",WMouseNoMask)
{
	if (!config.dock)
		return;
	QPixmap pix = *icons->loadIcon("offline");
	QLabel::setPixmap(pix);
	QToolTip::add(this, i18n("Left click - hide/show window\nMiddle click or CTRL+any click- next message"));
	// WindowMaker
	if(config.dock_wmaker)
	{
		resize(64,64);
		setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
		update();

		WMakerMasterWidget = new QWidget(0,"WMakerMasterWidget");
		WMakerMasterWidget->setGeometry(-10,-10,0,0);

		Display* dsp = x11Display();
		WId win = WMakerMasterWidget->winId();
		XWMHints* hints = XGetWMHints(dsp, win);
		hints->window_group = win;
		hints->icon_window = winId();
		hints->flags |= WindowGroupHint | IconWindowHint;
		XSetWMHints(dsp, win, hints);
		XFree( hints );		
	}
	// KDE/GNOME
	else
	{
		setWFlags(WRepaintNoErase);
		resize(22,22);
		update();
//		send_dock_request(x11Display(),this);
		Display *dsp = x11Display();
		WId win = winId();
		int r;
		int data = 1;
		r = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
		XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
		data = 0;
		r = XInternAtom(dsp, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
		XChangeProperty(dsp, win, r, XA_WINDOW, 32, 0, (uchar *)&data, 1);
	};
	//
	setBackgroundMode(X11ParentRelative);
	icon_timer = new QTimer(this);
	blink = FALSE;
	QObject::connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	hint = new TrayHint(0);
}

TrayIcon::~TrayIcon()
{
	if(config.dock_wmaker)
		delete WMakerMasterWidget;
	
	delete hint;
	
	kdebug("TrayIcon::~TrayIcon()\n");
}

void TrayIcon::setPixmap(const QPixmap& pixmap)
{
	QLabel::setPixmap(pixmap);
	if(config.dock_wmaker)
		WMakerMasterWidget->setIcon(pixmap);
	else
		repaint();
};

void TrayIcon::setType(QPixmap &pixmap)
{
	if (!config.dock)
		return;
	setPixmap(pixmap);
}

void TrayIcon::changeIcon() {
	if (pending.pendingMsgs() && config.dock && !icon_timer->isActive()) {
		if (!blink) {
			setPixmap(QPixmap((const char**)gg_msg_xpm));
			icon_timer->start(500,TRUE);
			blink = true;
			}
		else {
			setPixmap(*icons->loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
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
	else {
		pending.writeToFile();
		kadu->setClosePermitted(true);
		kadu->disconnectNetwork();
		kadu->close();
		}
}

void TrayIcon::show() {
	if(config.dock_wmaker)
		WMakerMasterWidget->show();
	else
		QLabel::show();
}

void TrayIcon::connectSignals() {
	QObject::connect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
}

void TrayIcon::mousePressEvent(QMouseEvent * e)
{
	int i,j, k = -1;
	QString tmp;
	PendingMsgs::Element elem;
	QString toadd;

	if (!config.dock)
		return;

	if (e->button() == MidButton || e->state() & ControlButton) {
		bool stop = false;
	
		UinsList uins;
		for (i = 0; i < pending.count(); i++) {
			elem = pending[i];
			if (!uins.count() || elem.uins.equals(uins))
				if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
					|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG) {
					if (!uins.count())
						uins = elem.uins;
					for (j = 0; j < elem.uins.count(); j++)
						if (!userlist.containsUin(elem.uins[j])) {
							tmp = QString::number(elem.uins[j]);
							if (config.dock)
								userlist.addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL,
									false, false, true, "", "", true);
							else
								kadu->addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL,
									"", "", true);
							}
					k = kadu->openChat(elem.uins);
					chats[k].ptr->formatMessage(false, userlist.byUin(elem.uins[0]).altnick,
						elem.msg, timestamp(elem.time), toadd);
					deletePendingMessage(i);
					i--;
					stop = true;
					}		
				else {
					if (!stop) {
						rMessage *rmsg;
						rmsg = new rMessage(userlist.byUin(elem.uins[0]).altnick,
							elem.msgclass, elem.uins, elem.msg);
						deletePendingMessage(i);
						UserBox::all_refresh();
						rmsg->init();
						rmsg->show();
						}
					else
						chats[k].ptr->scrollMessages(toadd);

					return;
					}
			}
		if (stop) {
			chats[k].ptr->scrollMessages(toadd);
	    		UserBox::all_refresh();
			return;
		}
	}

	if (e->button() == LeftButton)
		{
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
		dockppm->exec(QCursor::pos());
		return;
		}
}

void TrayIcon::showHint(const QString &str, const QString &nick, int index) {
	if (!config.trayhint || !config.dock)
		return;

	kdebug("TrayIcon::showHint()\n");
	hint->show_hint(str,nick,index);
}

void TrayIcon::reconfigHint() {
	kdebug("TrayIcon::reconfigHint()\n");
	hint->restart();
}

void TrayIcon::showErrorHint(const QString &str) {
	if (!config.hinterror)
		return;
	kdebug("TrayIcon::showErrorHint()\n");
	hint->show_hint(str, i18n("Error: "), 1);
}

TrayHint::TrayHint(QWidget *parent, const char *name)
	: QWidget(parent,"TrayHint",WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC)
{
	kdebug("TrayHint::TrayHint\n");
	
	hint = new QTextBrowser(this);
	hint->setVScrollBarMode(QScrollView::AlwaysOff);
	hint->setHScrollBarMode(QScrollView::AlwaysOff);
	hint->setFont(config.fonts.trayhint);
	hint->setPaletteBackgroundColor(config.colors.trayhintBg);
	hint->setPaletteForegroundColor(config.colors.trayhintText);

	hint_timer = new QTimer();
	
	QObject::connect(hint_timer,SIGNAL(timeout()),this,SLOT(remove_hint()));
}

void TrayHint::set_hint(void) {
	QPoint pos_hint;
	QSize size_hint;
	QPoint pos_tray = trayicon->mapToGlobal(QPoint(0, 0));
	QString text_hint; 
	for (QStringList::Iterator points = hint_list.begin(); points != hint_list.end(); ++points)
		text_hint.append(*points);
	size_hint = QFontMetrics(config.fonts.trayhint).size(Qt::ExpandTabs, text_hint);
	size_hint = QSize(size_hint.width()+35,size_hint.height()+10);
	resize(size_hint);
	hint->resize(size_hint);
	QSize size_desk = QApplication::desktop()->size();
	if (pos_tray.x() < size_desk.width()/2)
		pos_hint.setX(pos_tray.x()+32);
	else
		pos_hint.setX(pos_tray.x()-size_hint.width());
	if (pos_tray.y() < size_desk.height()/2)
		pos_hint.setY(pos_tray.y()+32);
	else
		pos_hint.setY(pos_tray.y()-size_hint.height());
	move(pos_hint);
	kdebug("TrayHint::set_hint()\n");
}

void TrayHint::show_hint(const QString &str, const QString &nick, int index) {
	kdebug("TrayHint::show_hint(%s,%s,%d)\n", 
		 (const char *)str.local8Bit(), (const char *)nick.local8Bit(), index);
	if (hint_list.last() == str + nick || hint_list.last() == "\n" + str + nick)
		return;
	QString text;
	text.append("<CENTER>");
	switch(index) {
		case 0:
			text.append(str);
			text.append("<B>");
			text.append(nick);
			text.append("</B>");
			break;

		case 1:
			text.append("<B>");
			text.append(nick);
			text.append("</B>");
			text.append(str);
			break;
	}

	text.append("</CENTER></FONT>");

	if (hint->text()=="") {
		hint->setText(text);
		hint_list.append(str+nick);
		}
	else {
		hint->setText(hint->text()+"\n"+text);
		hint_list.append("\n"+str+nick);
		}
	set_hint();
	show();
	if (!hint_timer->isActive())
		hint_timer->start(config.hinttime * 1000);
}

void TrayHint::remove_hint() {
	int len = hint->text().find("\n");
	if ( len > 0) {
		hint->setText(hint->text().remove(0, len + 1));
		hint_list.erase(hint_list.fromLast());
		kdebug("TrayHint::remove_hint() hint_list counts=%d\n",hint_list.count());
		}
	else {
		hide();
		hint->clear();
		hint_timer->stop();
		hint_list.clear();
		kdebug("TrayHint::remove_hint() hint and hint_list is cleared\n");
		return;
	}
	set_hint();
}

void TrayHint::restart() {
	hide();
	hint->clear();
	hint_timer->stop();
	hint_list.clear();
	hint->setFont(config.fonts.trayhint);
	hint->setPaletteBackgroundColor(config.colors.trayhintBg);
	hint->setPaletteForegroundColor(config.colors.trayhintText);
	kdebug("TrayHint::restart()\n");
}

TrayIcon *trayicon = NULL;
