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

extern "C" int docking_init()
{
	docking_manager = new DockingManager();
	return 0;
}

extern "C" void docking_close()
{
	kdebugf();
	delete docking_manager;
	docking_manager = NULL;
}

DockingManager::DockingManager()
{
	icon_timer = new QTimer(this);
	blink = false;
	QObject::connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));
	changeIcon();
	emit trayTooltipChanged(tr("Left click - hide/show window\nMiddle click or Left click- next message"));

	
	ConfigDialog::addCheckBox("General", "grid", 
			QT_TRANSLATE_NOOP("@default", "Start docked"), "RunDocked", false);
	
	connect(kadu, SIGNAL(connectingBlinkShowOffline()), this, SLOT(showOffline()));
	connect(kadu, SIGNAL(connectingBlinkShowStatus(int)), this, SLOT(showStatus(int)));
	connect(kadu, SIGNAL(currentStatusChanged(int)), this, SLOT(showCurrentStatus(int)));
	connect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	connect(hintmanager, SIGNAL(searchingForPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	kadu->setDocked(true);	
}

DockingManager::~DockingManager()
{
	kdebugf();

	ConfigDialog::removeControl("General", "Start docked");

	disconnect(kadu, SIGNAL(connectingBlinkShowOffline()), this, SLOT(showOffline()));
	disconnect(kadu, SIGNAL(connectingBlinkShowStatus(int)), this, SLOT(showStatus(int)));
	disconnect(kadu, SIGNAL(currentStatusChanged(int)), this, SLOT(showCurrentStatus(int)));
	disconnect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	disconnect(hintmanager, SIGNAL(searchingForPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	disconnect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	disconnect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	kadu->setDocked(false);
	delete icon_timer;
	icon_timer=NULL;
}

void DockingManager::changeIcon()
{
	if (pending.pendingMsgs() && !icon_timer->isActive())
	{
		if (!blink)
		{
			emit trayPixmapChanged(icons_manager.loadIcon("Message"));
			icon_timer->start(500,TRUE);
			blink = true;
		}
		else
		{
			emit trayPixmapChanged(icons_manager.loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
			icon_timer->start(500,TRUE);
			blink = false;
		}
	}
	else
		kdebug("DockWidget::changeIcon() OFF\n");
}

void DockingManager::dockletChange(int id)
{
	if (id < 9)
		kadu->slotHandleState(id);
	else
		kadu->close(true);
}

void DockingManager::pendingMessageAdded()
{
	changeIcon();
}

void DockingManager::pendingMessageDeleted()
{
	if (!pending.pendingMsgs())
	{
		QPixmap pix = icons_manager.loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
		emit trayPixmapChanged(pix);
	}
}

void DockingManager::showOffline()
{
	emit trayPixmapChanged(icons_manager.loadIcon("Offline"));
}

void DockingManager::showStatus(int status)
{
	int i = statusGGToStatusNr(status);
	emit trayPixmapChanged(icons_manager.loadIcon(gg_icons[i]));
}

void DockingManager::showCurrentStatus(int status)
{
	int statusnr = statusGGToStatusNr(status);
	QPixmap pix = icons_manager.loadIcon(gg_icons[statusnr]);
	if (!pending.pendingMsgs())
		emit trayPixmapChanged(pix);
}

void DockingManager::findTrayPosition(QPoint& pos)
{
	emit searchingForTrayPosition(pos);
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
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

QPixmap DockingManager::defaultPixmap()
{
	return icons_manager.loadIcon(gg_icons[
		statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]);
}

DockingManager* docking_manager = NULL;
