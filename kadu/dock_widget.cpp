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

#include "dock_widget.h"
#include "misc.h"
#include "pixmaps.h"
#include "message.h"
#include "chat.h"

//
#include "kadu.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

TrayIcon::TrayIcon(QWidget *parent, const char *name)
	: QLabel(0,"TrayIcon",WMouseNoMask)
{
	if (!config.dock)
		return;
	QPixmap pix=QPixmap((const char**)gg_inact_xpm);
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

		Display *dsp = x11Display();
		WId win = winId();
		int r;
		int data = 1;
		r = XInternAtom(dsp, "KWM_DOCKWINDOW", false);
		XChangeProperty(dsp, win, r, r, 32, 0, (uchar *)&data, 1);
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
	
	fprintf(stderr, "KK TrayIcon::~TrayIcon()\n");
}

void TrayIcon::setPixmap(const QPixmap& pixmap)
{
	QLabel::setPixmap(pixmap);
	if(config.dock_wmaker)
		WMakerMasterWidget->setIcon(pixmap);
	else
		repaint();
};

void TrayIcon::setType(char **gg_xpm)
{
	if (!config.dock)
		return;
	setPixmap(QPixmap((const char**)gg_xpm));
}

void TrayIcon::changeIcon() {
	if (pending.pendingMsgs() && config.dock && !icon_timer->isActive()) {
		if (!blink) {
			setPixmap(QPixmap((const char**)gg_msg_xpm));
			icon_timer->start(500,TRUE);
			blink = true;
			}
		else {
			setPixmap(QPixmap((const char**)gg_xpm[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
			icon_timer->start(500,TRUE);
			blink = false;
			}
	}
	else {
		fprintf(stderr, "KK DockWidget::changeIcon() OFF\n");
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
	fprintf(stderr,"KK TrayIcon::showHint()\n");
	hint->show_hint(str,nick,index);
}

TrayHint::TrayHint(QWidget *parent, const char *name)
	: QWidget(parent,"TrayHint",WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC)
{
	fprintf(stderr,"KK TrayHint::TrayHint\n");
	
	hint = new QTextBrowser(this);
	hint->setVScrollBarMode(QScrollView::AlwaysOff);
	hint->setHScrollBarMode(QScrollView::AlwaysOff);
	hint->setFont(config.fonts.userbox);
//	hint->setPaletteBackgroundColor("#FF00AA");

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
	size_hint = QFontMetrics(config.fonts.userbox).size(Qt::ExpandTabs, text_hint);
	size_hint = QSize(size_hint.width()+20,size_hint.height()+10);
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
	fprintf(stderr,"KK TrayHint::set_hint()\n");
}

void TrayHint::show_hint(const QString &str, const QString &nick, int index) {
	fprintf(stderr,"KK TrayHint::show_hint(%s,%s,%d)\n",str.latin1(),nick.latin1(),index);
	
	QString text;
	text.append("<FONT color=\"");
	text.append(config.colors.mychatText.name());
	text.append("\">");
	text.append("<CENTER>");
	if (index == 0) {
		text.append(str);
		text.append("<B>");
		text.append(nick);
		text.append("</B>");
		}
	else {
		text.append("<B>");
		text.append(nick);
		text.append("</B>");
		text.append(str);
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
		hint_timer->start(5000);
}

void TrayHint::remove_hint() {
	int len = hint->text().find("\n");
	if ( len > 0) {
		hint->setText(hint->text().remove(0, len + 1));
		hint_list.erase(hint_list.fromLast());
		fprintf(stderr, "KK TrayHint::remove_hint() hint_list counts=%d\n",hint_list.count());
		}
	else {
		hide();
		hint->clear();
		hint_timer->stop();
		hint_list.clear();
		fprintf(stderr, "KK TRayHint::remove_hint() hint and hint_list is cleared\n");
		return;
	}
	set_hint();
}

TrayIcon *trayicon = NULL;
