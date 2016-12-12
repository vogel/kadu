/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "config-wizard-actions.h"

#include "show-config-wizard-action.h"

#include "actions/actions.h"
#include "actions/action-description.h"
#include "menu/menu-inventory.h"
#include "widgets/configuration/configuration-widget.h"
#include "misc/paths-provider.h"
#include "debug.h"

#include <QtWidgets/QAction>

ConfigWizardActions::ConfigWizardActions(QObject *parent) :
		QObject{parent}
{
}

ConfigWizardActions::~ConfigWizardActions()
{
}

void ConfigWizardActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void ConfigWizardActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void ConfigWizardActions::setShowConfigWizardAction(ShowConfigWizardAction *showConfigWizardAction)
{
	m_showConfigWizardAction = showConfigWizardAction;
}

void ConfigWizardActions::registerActions()
{
	m_actions->insert(m_showConfigWizardAction);
	m_menuInventory
		->menu("tools")
		->addAction(m_showConfigWizardAction, KaduMenu::SectionTools)
		->update();
}

void ConfigWizardActions::unregisterActions()
{
	m_menuInventory
		->menu("tools")
		->removeAction(m_showConfigWizardAction)
		->update();
	m_actions->remove(m_showConfigWizardAction);
}

#include "moc_config-wizard-actions.cpp"
