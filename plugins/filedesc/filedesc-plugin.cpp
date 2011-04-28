/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/windows/main-configuration-window.h"
#include "misc/path-conversion.h"

#include "filedesc.h"

#include "filedesc-plugin.h"

FiledescPlugin::~FiledescPlugin()
{
}

int FiledescPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	FileDescriptionInstance = new FileDescription(this);
	MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/filedesc.ui"));

	return 0;
}

void FiledescPlugin::done()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/filedesc.ui"));
}

Q_EXPORT_PLUGIN2(filedesc, FiledescPlugin)
