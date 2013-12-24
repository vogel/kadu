/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "plugins/docking/docking.h"

#include "qt4_docking.h"

#include "qt4-docking-plugin.h"

Qt4DockingPlugin::~Qt4DockingPlugin()
{
}

bool Qt4DockingPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	DockingManager::instance()->setDocker(Qt4TrayIcon::createInstance());
	return true;
}

void Qt4DockingPlugin::done()
{
	DockingManager::instance()->setDocker(0);

	Qt4TrayIcon::destroyInstance();
}

Q_EXPORT_PLUGIN2(qt4_docking, Qt4DockingPlugin)

#include "moc_qt4-docking-plugin.cpp"
