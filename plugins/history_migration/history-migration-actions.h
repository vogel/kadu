/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_MIGRATION_ACTIONS_H
#define HISTORY_MIGRATION_ACTIONS_H

#include <QtCore/QObject>

class QAction;

class ActionDescription;

class HistoryMigrationActions : public QObject
{
	Q_OBJECT

	static HistoryMigrationActions *Instance;

	ActionDescription *ImportHistoryActionDescription;

	HistoryMigrationActions();
	~HistoryMigrationActions();

private slots:
	void importHistoryActionActivated(QAction *sender, bool toggled);

public:
	static void registerActions();
	static void unregisterActions();

	static HistoryMigrationActions * instance() { return Instance; }

	void runImportHistoryAction();

};

#endif // HISTORY_MIGRATION_ACTIONS_H
