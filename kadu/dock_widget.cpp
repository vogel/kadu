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
	QToolTip::add(this, i18n("Left click - hide/show window\nMiddle click - next message"));
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
      icon_timer->start(1000,TRUE);
      blink = true;
      }
    else {
      setPixmap(QPixmap((const char**)gg_xpm[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
      icon_timer->start(1000,TRUE);
      blink = false;
      }
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

	if (e->button() == MidButton) {
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
							kadu->addUser("", "", tmp, tmp, "", tmp, GG_STATUS_NOT_AVAIL, "", "", true);
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

DockWidget* dw;

#include "dock_widget.moc"
