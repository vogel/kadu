/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QAction>

#include "gui/menu/menu-inventory.h"
#include "gui/windows/kadu-window.h"

#include "autostatus-service.h"

#include "autostatus-actions.h"

AutostatusActions::AutostatusActions(QObject *parent) :
		QObject{parent},
		AutostatusActionDescription{}
{
}

AutostatusActions::~AutostatusActions()
{
}

void AutostatusActions::setAutostatusService(AutostatusService *autostatusService)
{
	m_autostatusService = autostatusService;
}

void AutostatusActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void AutostatusActions::init()
{
	registerActions();
}

void AutostatusActions::done()
{
	unregisterActions();
}

void AutostatusActions::registerActions()
{
	AutostatusActionDescription = new ActionDescription(this,
		ActionDescription::TypeMainMenu, "autostatusAction",
		this, SLOT(autostatusActionActivated(QAction *, bool)),
		KaduIcon(), tr("&Autostatus"), true
	);

	m_menuInventory
		->menu("main")
		->addAction(AutostatusActionDescription, KaduMenu::SectionMiscTools, 2)
		->update();
}

void AutostatusActions::unregisterActions()
{
	m_menuInventory
		->menu("main")
		->removeAction(AutostatusActionDescription)
		->update();

	delete AutostatusActionDescription;
	AutostatusActionDescription = 0;
}

void AutostatusActions::autostatusActionActivated(QAction *action, bool toggled)
{
	Q_UNUSED(action)

	m_autostatusService->toggle(toggled);
}

#include "moc_autostatus-actions.cpp"
