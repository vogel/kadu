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

DockingManager::DockingManager() : QObject(NULL, "docking_manager")
{
	kdebugf();
	icon_timer = new QTimer(this);
	blink = false;
	QObject::connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(kadu, SIGNAL(statusPixmapChanged(QPixmap &)),
		this, SLOT(statusPixmapChanged(QPixmap &)));
	connect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	hintmanager_existed_on_start=(hintmanager!=NULL);
	if (hintmanager_existed_on_start)
		connect(hintmanager, SIGNAL(searchingForPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	connect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();

	disconnect(kadu, SIGNAL(statusPixmapChanged(QPixmap &)),
		this, SLOT(statusPixmapChanged(QPixmap &)));
	disconnect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));
	if (hintmanager_existed_on_start && hintmanager!=NULL)
		disconnect(hintmanager, SIGNAL(searchingForPosition(QPoint&)), this, SLOT(findTrayPosition(QPoint&)));
	disconnect(dockppm, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	disconnect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	delete icon_timer;
	icon_timer=NULL;
	kdebugf2();
}

void DockingManager::changeIcon()
{
	kdebugf();
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
			emit trayPixmapChanged(gadu->status().pixmap());
			icon_timer->start(500,TRUE);
			blink = false;
		}
	}
	else
		kdebugm(KDEBUG_INFO, "DockingManager::changeIcon() OFF\n");
	kdebugf2();
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
		emit trayPixmapChanged(gadu->status().pixmap());
}

void DockingManager::defaultToolTip()
{
	QString tiptext = tr("Left click - hide/show window\nMiddle click or Left click- open message");
	tiptext.append(tr("\n\nCurrent status:\n%1")
		.arg(qApp->translate("@default", Status::name(gadu->status().index()))));

	if (gadu->status().hasDescription())
		tiptext.append(tr("\n\nDescription:\n%2").arg(gadu->status().description()));

	emit trayTooltipChanged(tiptext);
}

void DockingManager::findTrayPosition(QPoint& pos)
{
	emit searchingForTrayPosition(pos);
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == MidButton)
	{
		emit mousePressMidButton();
		return;
	}

	if (e->button() == LeftButton)
	{
		emit mousePressLeftButton();
		if (pending.pendingMsgs() && e->state() != ControlButton)
		{
			pending.openMessages();
			return;
		}
		switch (kadu->isVisible())
		{
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
	kdebugf2();
}

void DockingManager::statusPixmapChanged(QPixmap &pix)
{
 	kdebugf();
	emit trayPixmapChanged(pix);
	defaultToolTip();
}

QPixmap DockingManager::defaultPixmap()
{
	return gadu->status().pixmap();
}

void DockingManager::setDocked(bool docked)
{
	kdebugf();
	if(docked)
	{
		changeIcon();
		defaultToolTip();
		ConfigDialog::addCheckBox("General", "grid",
			QT_TRANSLATE_NOOP("@default", "Start docked"), "RunDocked", false);
		if (config_file.readBoolEntry("General", "RunDocked"))
			kadu->setShowMainWindowOnStart(false);
	}
	else
	{
		ConfigDialog::removeControl("General", "Start docked");
		kadu->show();
	}
	kadu->setDocked(docked);
	kdebugf2();
}

DockingManager* docking_manager = NULL;
