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
#include <qpopupmenu.h>
#include <qtimer.h>

#include "config_file.h"
#include "config_dialog.h"
#include "docking.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "status.h"
#include "misc.h"

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

	connect(kadu, SIGNAL(statusPixmapChanged(const QPixmap &, const QString &)),
		this, SLOT(statusPixmapChanged(const QPixmap &, const QString &)));
	connect(&pending, SIGNAL(messageAdded()), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageDeleted()), this, SLOT(pendingMessageDeleted()));

	connect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	connect(dockMenu, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	ConfigDialog::addCheckBox("General", "grid-advanced", QT_TRANSLATE_NOOP("@default", "Show tooltip in tray"),
			"ShowTooltipInTray", true, 0, 0, Advanced);
	ConfigDialog::addComboBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "New message tray icon"),
			"NewMessageIcon", toStringList(tr("Blinking envelope"), tr("Static envelope"), tr("Animated envelope")),
			toStringList("0", "1", "2"), "0", 0, 0, Advanced);
	ConfigDialog::registerSlotOnApplyTab("General", this, SLOT(onApplyTabGeneral()));
	ConfigDialog::registerSlotOnApplyTab("Look", this, SLOT(onApplyTabLook()));
	newMessageIcon = (IconType) config_file.readNumEntry("Look", "NewMessageIcon");

	kdebugf2();
}

void DockingManager::onApplyTabGeneral()
{
	kdebugf();
	if (ConfigDialog::getCheckBox("General", "Show tooltip in tray")->isChecked())
		defaultToolTip();
	else
		emit trayTooltipChanged(QString::null);
	kdebugf2();
}

void DockingManager::onApplyTabLook()
{
	kdebugf();
	IconType it = (IconType) config_file.readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();
	ConfigDialog::unregisterSlotOnApplyTab("General", this, SLOT(onApplyTabGeneral()));
	ConfigDialog::unregisterSlotOnApplyTab("Look", this, SLOT(onApplyTabLook()));
	ConfigDialog::removeControl("General", "Show tooltip in tray");
	ConfigDialog::removeControl("Look", "New message tray icon");

	disconnect(kadu, SIGNAL(statusPixmapChanged(const QPixmap &, const QString &)),
		this, SLOT(statusPixmapChanged(const QPixmap &, const QString &)));
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
				emit trayMovieChanged(icons_manager->loadAnimatedIcon("MessageAnim"));
				break;
			case StaticEnvelope:
				emit trayPixmapChanged(icons_manager->loadIcon("Message"), "Message");
				break;
			case BlinkingEnvelope:
				if (!blink)
				{
					emit trayPixmapChanged(icons_manager->loadIcon("Message"), "Message");
					icon_timer->start(500,TRUE);
					blink = true;
				}
				else
				{
					const UserStatus &stat = gadu->status();
					emit trayPixmapChanged(stat.pixmap(), stat.toString());
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
	{
		const UserStatus &stat = gadu->status();
		emit trayPixmapChanged(stat.pixmap(), stat.toString());
	}
}

void DockingManager::defaultToolTip()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		QString tiptext = tr("Left click - hide / show window\nMiddle click or Left click - open message");
		tiptext.append(tr("\n\nCurrent status:\n%1")
			.arg(qApp->translate("@default", UserStatus::name(gadu->status().index()).ascii())));

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
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());
		if (pending.pendingMsgs() && e->state() != ControlButton)
		{
			pending.openMessages();
			return;
		}
		if (kadu->isMinimized())
		{
			// hide needed when changing desktop
			kadu->hide();
			kadu->showNormal();
			kadu->setFocus();
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

		QSize desktopSize = QApplication::desktop()->size();
		QSize menuSizeHint = dockMenu->sizeHint();
		QPoint p = QCursor::pos();
//		kdebugm(KDEBUG_INFO, "p:%d,%d menuSize:%d,%d desktop:%d,%d\n", p.x(), p.y(), menuSizeHint.width(), menuSizeHint.height(), desktopSize.width(), desktopSize.height());
		if (p.y() + menuSizeHint.height() >= desktopSize.height())
			p.setY(p.y() - menuSizeHint.height() - 10);
		else
			p.setY(p.y() + 10);
		if (p.x() + menuSizeHint.width() >= desktopSize.width())
			p.setX(p.x() - menuSizeHint.width() - 10);
		else
			p.setX(p.x() + 10);
//		kdebugm(KDEBUG_INFO, "new_p:%d,%d\n", p.x(), p.y());

		dockMenu->exec(p);
		return;
	}
	kdebugf2();
}

void DockingManager::statusPixmapChanged(const QPixmap &icon, const QString &iconName)
{
 	kdebugf();
	emit trayPixmapChanged(icon, iconName);
	defaultToolTip();
	changeIcon();
}

QPixmap DockingManager::defaultPixmap()
{
	return gadu->status().pixmap();
}

void DockingManager::setDocked(bool docked, bool butDontHideOnClose)
{
	kdebugf();
	if (docked)
	{
		changeIcon();
		defaultToolTip();
		ConfigDialog::addCheckBox("General", "grid-beginner",
			QT_TRANSLATE_NOOP("@default", "Start docked"), "RunDocked", false, 0, 0, Beginner);
		if (config_file.readBoolEntry("General", "RunDocked"))
			kadu->setShowMainWindowOnStart(false);
	}
	else
	{
		kdebugm(KDEBUG_INFO, "closing: %d\n", Kadu::closing());
		ConfigDialog::removeControl("General", "Start docked");
		if (!Kadu::closing())
			kadu->show();
	}
	kadu->setDocked(docked, butDontHideOnClose);
	kdebugf2();
}

DockingManager* docking_manager = NULL;
