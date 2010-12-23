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

#include <QtGui/QAction>

#include "core/core.h"
#include "gui/windows/kadu-window.h"

#include "autostatus.h"

#include "autostatus-actions.h"

AutostatusActions * AutostatusActions::Instance = 0;

void AutostatusActions::createInstance()
{
	if (!Instance)
		Instance = new AutostatusActions();
}

void AutostatusActions::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

AutostatusActions::AutostatusActions()
{
}

AutostatusActions::~AutostatusActions()
{
}

void AutostatusActions::registerActions()
{
	AutostatusActionDescription = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "autostatusAction",
		this, SLOT(autostatusActionActivated(QAction *, bool)),
		QString(), tr("&Autostatus"), true
	);
	Core::instance()->kaduWindow()->insertMenuActionDescription(AutostatusActionDescription, KaduWindow::MenuKadu, 6);
}

void AutostatusActions::unregisterActions()
{
	Core::instance()->kaduWindow()->removeMenuActionDescription(AutostatusActionDescription);
	delete AutostatusActionDescription;
	AutostatusActionDescription = 0;
}

void AutostatusActions::autostatusActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)

	Autostatus::instance()->toggle(toggled);
}
