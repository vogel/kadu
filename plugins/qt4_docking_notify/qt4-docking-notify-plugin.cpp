/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QSystemTrayIcon>

#include "qt4-docking-notify.h"

#include "qt4-docking-notify-plugin.h"

Qt4DockingNotifyPlugin::~Qt4DockingNotifyPlugin()
{
}

int Qt4DockingNotifyPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	if (!QSystemTrayIcon::supportsMessages())
		return 1;

	NotifierInstance = new Qt4Notify(this);

	return 0;
}

void Qt4DockingNotifyPlugin::done()
{
	delete NotifierInstance;
	NotifierInstance = 0;
}

Q_EXPORT_PLUGIN2(qt4_docking_notify, Qt4DockingNotifyPlugin)
