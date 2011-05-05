/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QFile>

#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "misc/path-conversion.h"
#include "protocols/protocol.h"

#include "history-importer.h"
#include "history-importer-manager.h"

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

HistoryMigrationActions::HistoryMigrationActions() :
		ImportHistoryActionDescription(0)
{
	bool imported = config_file.readBoolEntry("History", "Imported_from_0.6.5", false);
	Account gaduAccount = AccountManager::instance()->byId("gadu", config_file.readEntry("General", "UIN"));
	if (!imported && gaduAccount && QFile::exists(profilePath("history")))
	{
		ImportHistoryActionDescription = new ActionDescription(this, ActionDescription::TypeGlobal, "import_history",
				this, SLOT(importHistoryActionActivated(QAction*,bool)), KaduIcon(), tr("Import history..."));

		Core::instance()->kaduWindow()->insertMenuActionDescription(ImportHistoryActionDescription, KaduWindow::MenuTools);
	}
}

HistoryMigrationActions::~HistoryMigrationActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(ImportHistoryActionDescription);
}

void HistoryMigrationActions::importHistoryActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)

	runImportHistoryAction();
}

void HistoryMigrationActions::runImportHistoryAction()
{
	if (!ImportHistoryActionDescription)
		return;

	if (HistoryImporterManager::instance()->containsImporter(profilePath("history/")))
		return;

	Account gaduAccount = AccountManager::instance()->byId("gadu", config_file.readEntry("General", "UIN"));
	if (!gaduAccount)
		return;

	HistoryImporter *hi = new HistoryImporter(gaduAccount, profilePath("history/"));
	HistoryImporterManager::instance()->addImporter(hi);

	hi->run();
}
