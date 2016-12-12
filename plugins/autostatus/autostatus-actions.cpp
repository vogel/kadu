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

#include "autostatus-actions.h"

#include "autostatus-service.h"
#include "toggle-autostatus-action.h"

#include "actions/actions.h"
#include "menu/menu-inventory.h"

AutostatusActions::AutostatusActions(QObject *parent) :
		QObject{parent}
{
}

AutostatusActions::~AutostatusActions()
{
}

void AutostatusActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void AutostatusActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void AutostatusActions::setToggleAutostatusAction(ToggleAutostatusAction *toggleAutostatusAction)
{
	m_toggleAutostatusAction = toggleAutostatusAction;
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
	m_actions->insert(m_toggleAutostatusAction);
	m_menuInventory
		->menu("main")
		->addAction(m_toggleAutostatusAction, KaduMenu::SectionMiscTools, 2)
		->update();
}

void AutostatusActions::unregisterActions()
{
	m_menuInventory
		->menu("main")
		->removeAction(m_toggleAutostatusAction)
		->update();
	m_actions->remove(m_toggleAutostatusAction);
}

#include "moc_autostatus-actions.cpp"
