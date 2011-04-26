/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AUTOSTATUS_ACTIONS_H
#define AUTOSTATUS_ACTIONS_H

#include <QtCore/QObject>

class QAction;

class ActionDescription;

class AutostatusActions : public QObject
{
	Q_OBJECT

	static AutostatusActions *Instance;

	ActionDescription *AutostatusActionDescription;

	AutostatusActions();
	virtual ~AutostatusActions();

private slots:
	//! This slot is called when new Autostatus it starts
	void autostatusActionActivated(QAction *, bool);

public:
	static void createInstance();
	static void destroyInstance();

	static AutostatusActions * instance() { return Instance; }

	void registerActions();
	void unregisterActions();

};

#endif // AUTOSTATUS_ACTIONS_H
