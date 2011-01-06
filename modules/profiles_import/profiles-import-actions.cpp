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

#include <QtGui/QFileDialog>

#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"

#include "profile-importer.h"

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

ProfilesImportActions::ProfilesImportActions()
{
	ImportExternalProfile = new ActionDescription(this, ActionDescription::TypeGlobal, "import_external_profile",
			this, SLOT(importExternalProfileActionActivated(QAction*, bool)), QString(),
			tr("Import external profile..."), false);
	Core::instance()->kaduWindow()->insertMenuActionDescription(ImportExternalProfile, KaduWindow::MenuTools);
}

ProfilesImportActions::~ProfilesImportActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(ImportExternalProfile);
}

void ProfilesImportActions::importExternalProfileActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)
	Q_UNUSED(toggled)

	while (true)
	{
		QString directoryPath = QFileDialog::getExistingDirectory(Core::instance()->kaduWindow(), tr("Select profile directory"), QString());
		if (directoryPath.isEmpty())
			break; // cancel selected

		QFileInfo kaduConfFile(directoryPath + "/kadu.conf.xml");

		if (!kaduConfFile.exists())
		{
			MessageDialog::exec("dialog-warning", tr("Import external profile..."), tr("This directory is not a Kadu profile directory.\nFile kadu.conf.xml not found"));
			continue;
		}

		ProfileImporter importer(kaduConfFile.absoluteFilePath());
		if (importer.import())
			MessageDialog::exec("dialog-information", tr("Import external profile..."), tr("Profile successfully importer!"));
		else
			MessageDialog::exec("dialog-warning", tr("Import external profile..."), tr("Unable to import profile: %1").arg(importer.errorMessage()));

		return;
	}
}
