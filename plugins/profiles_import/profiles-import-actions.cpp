/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"
#include "gui/windows/import-profile-window.h"
#include "gui/windows/import-profiles-window.h"
#include "gui/windows/kadu-window.h"
#include "profile-data-manager.h"

#include "profiles-import-actions.h"

ProfilesImportActions * ProfilesImportActions::Instance = 0;

void ProfilesImportActions::registerActions()
{
	if (!Instance)
		Instance = new ProfilesImportActions();
}

void ProfilesImportActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

ProfilesImportActions * ProfilesImportActions::instance()
{
	return Instance;
}

ProfilesImportActions::ProfilesImportActions() :
		ImportProfiles(0)
{
	if (!ProfileDataManager::readProfileData().isEmpty())
	{
		Actions::instance()->blockSignals();

		ImportProfiles = new ActionDescription(this, ActionDescription::TypeGlobal, "import_profiles",
				this, SLOT(importProfilesActionActivated(QAction*, bool)), KaduIcon(),
				tr("Import profiles..."), false);

		MenuInventory::instance()
			->menu("tools")
			->addAction(ImportProfiles, KaduMenu::SectionTools)
			->update();

		// The last ActionDescription will send actionLoaded() signal.
		Actions::instance()->unblockSignals();
	}

	ImportExternalProfile = new ActionDescription(this, ActionDescription::TypeGlobal, "import_external_profile",
			this, SLOT(importExternalProfileActionActivated(QAction*, bool)), KaduIcon(),
			tr("Import external profile..."), false);

	MenuInventory::instance()
		->menu("tools")
		->addAction(ImportExternalProfile, KaduMenu::SectionTools)
		->update();
}

ProfilesImportActions::~ProfilesImportActions()
{
	MenuInventory::instance()
		->menu("tools")
		->removeAction(ImportProfiles)
		->update();
	MenuInventory::instance()
		->menu("tools")
		->removeAction(ImportExternalProfile)
		->update();
}

void ProfilesImportActions::updateActions()
{
	if (ProfileDataManager::readProfileData().isEmpty())
	{
		MenuInventory::instance()
			->menu("tools")
			->removeAction(ImportProfiles)
			->update();
	}
}

void ProfilesImportActions::importProfilesActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)
	Q_UNUSED(toggled)

	(new ImportProfilesWindow(Core::instance()->kaduWindow()))->show();
}

void ProfilesImportActions::importExternalProfileActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)
	Q_UNUSED(toggled)

	(new ImportProfileWindow(Core::instance()->kaduWindow()))->show();
}

#include "moc_profiles-import-actions.cpp"
