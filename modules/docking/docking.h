/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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
#include <QtGui/QIcon>

#include "configuration/configuration-aware-object.h"
#include "status/status-container-aware-object.h"

#include "docking_exports.h"

class QAction;
class QMenu;

class StatusContainer;

/**
 * @defgroup docking Docking
 * @{
 */
class DOCKINGAPI DockingManager : public QObject, ConfigurationAwareObject, StatusContainerAwareObject
{
	Q_OBJECT

	QMenu *DockMenu;
	QAction *CloseKaduAction;
	QAction *containersSeparator;
    #ifdef Q_OS_MAC
	QAction *OpenChatAction;
    #endif

	QMap<StatusContainer *, QAction *> StatusContainerMenus;

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;
	void defaultToolTip();

	void updateContextMenu();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

private slots:
	void statusPixmapChanged(const QIcon &icon);
	void changeIcon();
	void pendingMessageAdded();
	void pendingMessageDeleted();

	void containerStatusChanged();

public:
	DockingManager();
	virtual ~DockingManager();

	void trayMousePressEvent(QMouseEvent * e);
	QIcon defaultPixmap();
	QMenu * dockMenu() { return DockMenu; }

public slots:
	/**
		Modu� implementuj�cy dokowanie powinien to ustawic
		na true przy starcie i false przy zamknieciu, aby
		kadu wiedzialo, ze jest zadokowane.
	**/
	void setDocked(bool docked);

 signals:
	void trayPixmapChanged(const QIcon& icon);
	void trayMovieChanged(const QString& movie);
	void trayTooltipChanged(const QString& tooltip);
	void searchingForTrayPosition(QPoint& pos);
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();
};

extern DOCKINGAPI DockingManager* docking_manager;

/** @} */

#endif // DOCKING_H
