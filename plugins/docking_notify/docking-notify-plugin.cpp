/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QSystemTrayIcon>

#include "docking-notify.h"

#include "docking-notify-plugin.h"

DockingNotifyPlugin::~DockingNotifyPlugin()
{
}

bool DockingNotifyPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (!QSystemTrayIcon::supportsMessages())
		return false;

	NotifierInstance = new DockingNotify(this);

	return true;
}

void DockingNotifyPlugin::done()
{
	delete NotifierInstance;
	NotifierInstance = 0;
}

Q_EXPORT_PLUGIN2(qt4_docking_notify, DockingNotifyPlugin)

#include "moc_docking-notify-plugin.cpp"
