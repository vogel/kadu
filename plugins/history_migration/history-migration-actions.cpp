/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/actions/action-description.h"
#include "gui/menu/menu-inventory.h"
#include "misc/paths-provider.h"
#include "protocols/protocol.h"

#include "history-importer-manager.h"
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

HistoryMigrationActions::HistoryMigrationActions() :
		ImportHistoryActionDescription(0)
{
	bool imported = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "Imported_from_0.6.5", false);
	Account gaduAccount = AccountManager::instance()->byId("gadu", Application::instance()->configuration()->deprecatedApi()->readEntry("General", "UIN"));
	if (!imported && gaduAccount && QFile::exists(Application::instance()->pathsProvider()->profilePath() + QLatin1String("history")))
	{
		ImportHistoryActionDescription = new ActionDescription(this, ActionDescription::TypeGlobal, "import_history",
				this, SLOT(importHistoryActionActivated(QAction*,bool)), KaduIcon(), tr("Import history..."));

		MenuInventory::instance()
			->menu("tools")
			->addAction(ImportHistoryActionDescription, KaduMenu::SectionTools)
			->update();
	}
}

HistoryMigrationActions::~HistoryMigrationActions()
{
	MenuInventory::instance()
		->menu("tools")
		->removeAction(ImportHistoryActionDescription)
		->update();
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

	if (HistoryImporterManager::instance()->containsImporter(Application::instance()->pathsProvider()->profilePath() + QLatin1String("history/")))
		return;

	Account gaduAccount = AccountManager::instance()->byId("gadu", Application::instance()->configuration()->deprecatedApi()->readEntry("General", "UIN"));
	if (!gaduAccount)
		return;

	HistoryImporter *hi = new HistoryImporter(gaduAccount, Application::instance()->pathsProvider()->profilePath() + QLatin1String("history/"));
	HistoryImporterManager::instance()->addImporter(hi);

	hi->run();
}

#include "moc_history-migration-actions.cpp"
