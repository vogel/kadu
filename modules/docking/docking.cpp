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
#include <qcheckbox.h>
#include <qobject.h>
#include <qtimer.h>

#include "config_file.h"
#include "config_dialog.h"
#include "docking.h"
#include "debug.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "status.h"

extern "C" int docking_init()
{
	docking_manager = new DockingManager(NULL, "docking_manager");
	return 0;
}

extern "C" void docking_close()
{
	kdebugf();
	delete docking_manager;
	docking_manager = NULL;
}

DockingManager::DockingManager(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	icon_timer = new QTimer(this, "icon_timer");
	blink = false;
	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(kadu, SIGNAL(statusPixmapChanged(QPixmap &)),
		this, SLOT(statusPixmapChanged(QPixmap &)));
	connect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));

	connect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	connect(dockMenu, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	ConfigDialog::addCheckBox("General", "grid", QT_TRANSLATE_NOOP("@default", "Show tooltip in tray"),
			"ShowTooltipInTray", true, "", "", Advanced);
	ConfigDialog::addComboBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "New message tray icon"),
			"NewMessageIcon", toStringList(tr("Blinking envelope"), tr("Static envelope"), tr("Animated envelope")),
			toStringList("0", "1", "2"), "0", "", "", Advanced);
	ConfigDialog::registerSlotOnApply(this, SLOT(onApplyConfigDialog()));
	newMessageIcon = (IconType) config_file.readNumEntry("Look", "NewMessageIcon");
	
	kdebugf2();
}

void DockingManager::onApplyConfigDialog()
{
	kdebugf();
	newMessageIcon = (IconType) config_file.readNumEntry("Look", "NewMessageIcon");
	if (ConfigDialog::getCheckBox("General", "Show tooltip in tray")->isChecked())
		defaultToolTip();
	else
		emit trayTooltipChanged("");
	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnApply(this, SLOT(onApplyConfigDialog()));
	ConfigDialog::removeControl("General", "Show tooltip in tray");

	disconnect(kadu, SIGNAL(statusPixmapChanged(QPixmap &)),
		this, SLOT(statusPixmapChanged(QPixmap &)));
	disconnect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));

	disconnect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	disconnect(dockMenu, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
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
		switch (newMessageIcon)
		{
			case AnimatedEnvelope:
				emit trayMovieChanged(icons_manager.loadAnimatedIcon("MessageAnim"));
				break;
			case StaticEnvelope:
				emit trayPixmapChanged(icons_manager.loadIcon("Message"));
				break;
			case BlinkingEnvelope:
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
				break;
		}
	}
	else
		kdebugmf(KDEBUG_INFO, "OFF\n");
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
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		QString tiptext = tr("Left click - hide/show window\nMiddle click or Left click- open message");
		tiptext.append(tr("\n\nCurrent status:\n%1")
			.arg(qApp->translate("@default", UserStatus::name(gadu->status().index()))));

		if (gadu->status().hasDescription())
			tiptext.append(tr("\n\nDescription:\n%2").arg(gadu->status().description()));

		emit trayTooltipChanged(tiptext);
	}
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

	if (e->button() == RightButton)
	{
		emit mousePressRightButton();
		dockMenu->exec(QCursor::pos());
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
	if (docked)
	{
		changeIcon();
		defaultToolTip();
		ConfigDialog::addCheckBox("General", "grid",
			QT_TRANSLATE_NOOP("@default", "Start docked"), "RunDocked", false, "", "", Advanced);
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
