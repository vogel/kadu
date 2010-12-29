/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef STATUS_MENU_H
#define STATUS_MENU_H

#include <QtCore/QObject>
#include <QtCore/QPoint>

#include "status/status.h"

#include "exports.h"

class QAction;
class QActionGroup;
class QMenu;

class StatusActions;
class StatusContainer;

class KADUAPI StatusMenu : public QObject
{
	Q_OBJECT

	QMenu *Menu;
	StatusActions *Actions;

	QPoint MousePositionBeforeMenuHide;

	StatusContainer *MyStatusContainer;

private slots:
	void aboutToHide();
	void changeStatus(QAction *action);
	void changeDescription();

public:
	explicit StatusMenu(StatusContainer *statusContainer, QMenu *menu, bool commonStatusIcons = false);
	virtual ~StatusMenu();

};

#endif // STATUS_MENU_H
