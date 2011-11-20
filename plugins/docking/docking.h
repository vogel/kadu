/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DOCKING_H
#define DOCKING_H

#include <QtCore/QMap>
#include <QtGui/QLabel>

#include "configuration/configuration-aware-object.h"
#include "status/status-container-aware-object.h"

#include "docking_exports.h"

class QAction;
class QMenu;

class Docker;
class KaduIcon;
class StatusContainer;
class StatusIcon;
class StatusMenu;

class DOCKINGAPI DockingManager : public QObject, ConfigurationAwareObject, StatusContainerAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DockingManager)

	static DockingManager *Instance;

	Docker *CurrentDocker;

#ifdef Q_WS_X11
	bool KaduWindowLastTimeVisible;
#endif
	bool DockMenuNeedsUpdate;
	QMenu *DockMenu;
#ifdef Q_OS_MAC
	QMenu *MacDockMenu;
#endif

	StatusMenu *AllAccountsMenu;

#ifdef Q_WS_X11
	QAction *ShowKaduAction;
	QAction *HideKaduAction;
#endif
	QAction *CloseKaduAction;
	QAction *containersSeparator;

	StatusIcon *Icon;

	QList<QAction *> ModulesActions;

	QMap<StatusContainer *, QAction *> StatusContainerMenus;

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;
	void defaultToolTip();

	void createDefaultConfiguration();

	DockingManager();
	virtual ~DockingManager();

	void doUpdateContextMenu();
	void openUnreadMessages();

private slots:
	void statusIconChanged(const KaduIcon &icon);
	void changeIcon();
	void unreadMessageAdded();
	void unreadMessageRemoved();
	void searchingForTrayPosition(QPoint &point);
	void iconThemeChanged();

	void showKaduWindow();
	void hideKaduWindow();

	void contextMenuAboutToBeShown();
	void updateContextMenu();

	void containerStatusChanged();

protected:
	virtual void configurationUpdated();
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	static void createInstance();
	static void destroyInstance();
	static DockingManager * instance();

	void trayMousePressEvent(QMouseEvent * e);
	KaduIcon defaultIcon();
	QMenu * dockMenu() { return DockMenu; }

	void setDocker(Docker *docker);

#ifdef Q_OS_MAC
	void showMinimizedChats();
	void dockIconClicked();
#endif

	void registerModuleAction(QAction *action);
	void unregisterModuleAction(QAction *action);

signals:
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();

};

#endif // DOCKING_H
