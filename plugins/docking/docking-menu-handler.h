/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "docking-exports.h"

#include <QtCore/QMap>
#include <QtCore/QObject>

class DockingMenuActionRepository;
class IconsManager;
class NotificationService;
class StatusContainer;
class StatusContainerManager;

class QAction;
class QMenu;

class DOCKINGAPI DockingMenuHandler final : public QObject
{
	Q_OBJECT

public:
	explicit DockingMenuHandler(QMenu *menu, QObject *parent = nullptr);
	virtual ~DockingMenuHandler();

	void setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository);
	void setIconsManager(IconsManager *iconsManager);
	void setNotificationService(NotificationService *notificationService);
	void setStatusContainerManager(StatusContainerManager *statusContainerManager);

private:
	DockingMenuActionRepository *m_dockingMenuActionRepository;
	NotificationService *m_notificationService;
	StatusContainerManager *m_statusContainerManager;

	QMenu *m_menu;
#if defined(Q_OS_UNIX)
	QAction *m_showKaduAction;
	QAction *m_hideKaduAction;
#endif
	QAction *m_silentModeAction;
	QAction *m_closeKaduAction;

	bool m_needsUpdate;
	bool m_mainWindowLastVisible;

	void doUpdate();
	void addStatusContainerMenus();
	void addStatusContainerMenu(StatusContainer *statusContainer);
	void addActionRepositoryMenus();

private slots:
	void aboutToShow();
	void update();

	void statusContainerRegistered(StatusContainer *statusContainer);
	void statusContainerUnregistered(StatusContainer *statusContainer);

	void showKaduWindow();
	void hideKaduWindow();
	void silentModeToggled(bool enabled);

};
