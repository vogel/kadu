/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dock_widget.h"
#include "misc.h"
#include "pixmaps.h"
#include "message.h"
#include "chat.h"

#include <klocale.h>
#include <qcursor.h>

//
#include "kadu.h"

DockWidget::DockWidget(QWidget *parent, const char *name ) : KSystemTray( parent, name )
{
	if (!config.dock)
		return;

	setPixmap( QPixmap((const char**)gg_inact_xpm) );
	QToolTip::add(this, i18n("Left click - hide/show window\nMiddle click or CTRL+any click- next message"));
	icon_timer = new QTimer(this);
	blink = FALSE;
	connect(icon_timer,SIGNAL(timeout()),this,SLOT(changeIcon()));
}

void DockWidget::setType(char **gg_xpm) {
	if (!config.dock)
		return;
	
	setPixmap(QPixmap((const char**)gg_xpm));
}

void DockWidget::changeIcon() {
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
		fprintf(stderr,"KK DockWidget::changeIcon OFF\n");
	}
}

void DockWidget::dockletChange(int id)
{
	if (id < 9)
		kadu->slotHandleState(id);
	else {
		pending.writeToFile();
		kadu->setClosePermitted(true);
		kadu->disconnectNetwork();
		kadu->close();
//		a->quit();
		}
}

void DockWidget::mousePressEvent(QMouseEvent * e) {
	bool message = false;
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
				if (elem.msgclass == GG_CLASS_CHAT || elem.msgclass == GG_CLASS_MSG) {
					if (!uins.count())
						uins = elem.uins;
					for (j = 0; j < elem.uins.count(); j++)
						if (!userlist.containsUin(elem.uins[j])) {
							tmp = QString::number(elem.uins[j]);
							userlist.addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL,
								false, false, true, "", "", true);
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

DockHint::DockHint(QWidget *parent) : QLabel(parent,"docktip",WStyle_NoBorder|WStyle_StaysOnTop|WStyle_Tool|WX11BypassWM|WWinOwnDC)
{
	fprintf(stderr,"KK DockHint::DockHint\n");
	remove_timer = new QTimer(this);
	setAlignment(Qt::AlignCenter);
	setPaletteBackgroundColor(QColor(255,255,230));
	setFrameStyle(QFrame::Box|QFrame::Plain);
	setLineWidth(1);
//	setMargin(2);
	
	connect(remove_timer,SIGNAL(timeout()),this,SLOT(remove_hint()));
}

void DockHint::Show(QString Text) {
	fprintf(stderr,"KK DockHint::Show(%s)\n",Text.latin1());
	if (text()=="")
		setText(Text);
	else
		setText(text()+"\n"+Text);
	
//Zamotany kod, nie probowac nawet go zrozumiec ;)
	QPoint p=dw->mapToGlobal(QPoint(0,0));
	QSize size=QFontMetrics(QFont(config.fonts.userboxFont,config.fonts.userboxFontSize)).size(Qt::ExpandTabs,text());
//trzeba dodac kilka pixeli bo to gowno wyzej nie dziala jak trzeba, na przyszlosc trzeba bedzie cos lepszego wymyslec
	size=QSize(size.width()+5,size.height()+5);
	fprintf(stderr,"w:%d,h:%d\n",size.width(),size.height());
	resize(size.width(),size.height());
	QSize desksize=QApplication::desktop()->size();

	if (p.x()+size.width() > desksize.width()) {
		if(p.x()-size.width() <0)
			p.setX(p.x()+dw->rect().width());
		else
			p.setX(p.x()-size.width());
	}
	else {
		if(p.x()-size.width() <0)
			p.setX(p.x()+dw->rect().width());
		else
			p.setX(p.x()-size.width());
	}
	if (p.y()-size.height()-dw->rect().height() < 0)
		p.setY(p.y()+dw->rect().height()/2+size.height());
	else
		p.setY(p.y()-dw->rect().height()/2-size.height());
//Koniec zamotanego kodu, otrzymalismy calkiem dobre wspolrzedne na docktip
	
	move(p);
	show();
	if (!remove_timer->isActive())
		remove_timer->start(5000);
}

void DockHint::remove_hint() {
	fprintf(stderr,"DockWidget::remove_hint()\n");
	int len = text().find('\n');
	fprintf(stderr,"len=%d\n",len);
	if (len > 0)
		setText(text().remove(0,len+1));
	else {
		hide();
		clear();
		remove_timer->stop();
		return;
	}
//zamotany kod
	QPoint p=dw->mapToGlobal(QPoint(0,0));
	QSize size=QFontMetrics(QFont(config.fonts.userboxFont,config.fonts.userboxFontSize)).size(Qt::ExpandTabs,text());
//trzeba dodac kilka pixeli bo to gowno wyzej nie dziala jak trzeba, na przyszlosc trzeba bedzie cos lepszego wymyslec
	size=QSize(size.width()+5,size.height()+5);
	fprintf(stderr,"w:%d,h:%d\n",size.width(),size.height());
	resize(size.width(),size.height());
	QSize desksize=QApplication::desktop()->size();

	if (p.x()+size.width() > desksize.width()) {
		if(p.x()-size.width() <0)
			p.setX(p.x()+dw->rect().width());
		else
			p.setX(p.x()-size.width());
	}
	else {
		if(p.x()-size.width() <0)
			p.setX(p.x()+dw->rect().width());
		else
			p.setX(p.x()-size.width());
	}
	if (p.y()-size.height()-dw->rect().height() < 0)
		p.setY(p.y()+dw->rect().height()/2+size.height());
	else
		p.setY(p.y()-dw->rect().height()/2-size.height());
//Koniec zamotanego kodu, otrzymalismy calkiem dobre wspolrzedne na docktip
	
	move(p);
}

DockWidget* dw;
DockHint *tip;
#include "dock_widget.moc"
