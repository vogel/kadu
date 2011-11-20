/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include "gui/windows/kadu-window.h"
#include "exports.h"

#include "gui/windows/import-profiles-window.h"
#include "profile-data-manager.h"

#include "profiles-import-actions.h"

#include "profiles-import-plugin.h"

ProfilesImportPlugin::~ProfilesImportPlugin()
{
}

int ProfilesImportPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ProfilesImportActions::registerActions();

	if (firstLoad && !ProfileDataManager::readProfileData().isEmpty())
		(new ImportProfilesWindow(Core::instance()->kaduWindow()))->show();

	return 0;
}

void ProfilesImportPlugin::done()
{
	ProfilesImportActions::unregisterActions();
}

Q_EXPORT_PLUGIN2(profiles_import, ProfilesImportPlugin)
