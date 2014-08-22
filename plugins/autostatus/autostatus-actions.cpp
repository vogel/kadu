/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QAction>

#include "gui/menu/menu-inventory.h"
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

AutostatusActions::AutostatusActions() :
		AutostatusActionDescription{}
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
		KaduIcon(), tr("&Autostatus"), true
	);

	MenuInventory::instance()
		->menu("main")
		->addAction(AutostatusActionDescription, KaduMenu::SectionMiscTools, 2)
		->update();
}

void AutostatusActions::unregisterActions()
{
	MenuInventory::instance()
		->menu("main")
		->removeAction(AutostatusActionDescription)
		->update();

	delete AutostatusActionDescription;
	AutostatusActionDescription = 0;
}

void AutostatusActions::autostatusActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)

	Autostatus::instance()->toggle(toggled);
}

#include "moc_autostatus-actions.cpp"
