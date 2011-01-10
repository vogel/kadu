/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"

#include "history-importer.h"

#include "history-migration-actions.h"

HistoryMigrationActions * HistoryMigrationActions::Instance = 0;

void HistoryMigrationActions::registerActions()
{
	if (!Instance)
		Instance = new HistoryMigrationActions();
}

void HistoryMigrationActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

HistoryMigrationActions::HistoryMigrationActions()
{
	ImportHistoryActionDescription = new ActionDescription(this, ActionDescription::TypeGlobal, "import_history",
			this, SLOT(importHistoryActionActivated(QAction*,bool)), QString(), tr("Import history..."));

	Core::instance()->kaduWindow()->insertMenuActionDescription(ImportHistoryActionDescription, KaduWindow::MenuTools);
}

HistoryMigrationActions::~HistoryMigrationActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(ImportHistoryActionDescription);
}

void HistoryMigrationActions::importHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	HistoryImporter::instance()->run();
}
